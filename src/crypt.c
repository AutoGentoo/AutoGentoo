//
// Created by atuser on 9/20/18.
//

#include <autogentoo/server.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <autogentoo/crypt.h>
#include <string.h>
#include <openssl/err.h>
#include <autogentoo/writeconfig.h>
#include <sys/stat.h>
#include <fcntl.h>

void x509_generate(int serial, int days_valid, X509** cert_out, EVP_PKEY* key_pair_evp) {
	X509_NAME* name = NULL;
	
	int status = 0;
	
	const unsigned char* country = (unsigned char*)"United States of America";
	const unsigned char* organization = (unsigned char*)"AutoGentoo";
	const unsigned char* common = (unsigned char*)"autogentoo";
	do {
		// Create the certificate object
		*cert_out = X509_new();
		if (!*cert_out) {
			lerror ("Failed to initialize certificate");
			break;
		}
		
		X509_set_version (*cert_out, 3);
		
		// Set the certificate's properties
		ASN1_INTEGER_set (X509_get_serialNumber (*cert_out), serial);
		X509_gmtime_adj (X509_get_notBefore (*cert_out), 0);
		X509_gmtime_adj (X509_get_notAfter (*cert_out), (long)(60 * 60 * 24 * (days_valid ? days_valid : 1)));
		name = X509_get_subject_name (*cert_out);
		
		X509_NAME_add_entry_by_txt (name, "C", MBSTRING_ASC, country, -1, -1, 0);
		X509_NAME_add_entry_by_txt (name, "CN", MBSTRING_ASC, common, -1, -1, 0);
		X509_NAME_add_entry_by_txt (name, "O", MBSTRING_ASC, organization, -1, -1, 0);
		X509_set_issuer_name (*cert_out, name);
		
		// Set the certificate's public key from the private key object
		if (!X509_set_pubkey (*cert_out, key_pair_evp)) {
			lerror("Failed to assign private key to certificate");
			break;
		}
		
		// Sign it with SHA-1
		if (!X509_sign (*cert_out, key_pair_evp, EVP_sha256())) {
			lerror("Failed to sign certificate with EVP_sha256()");
			break;
		}
		
		status = 1;
	} while (!status);
	
	// Things we clean up only on failure
	if (status == 0) {
		X509_free (*cert_out);
		*cert_out = NULL;
	}
}

int cert_generate_serial () {
	return 2;
}

int rsa_generate(EVP_PKEY** target) {
	int ret = 0;
	BIGNUM* bne = NULL;
	BIO* bp_public = NULL, * bp_private = NULL;
	*target = EVP_PKEY_new();
	
	int bits = AUTOGENTOO_RSA_BITS;
	linfo("Create new RSA %d key", AUTOGENTOO_RSA_BITS);
	unsigned long e = RSA_F4;
	
	// 1. generate rsa key
	bne = BN_new();
	ret = BN_set_word(bne, e);
	if (ret != 1) {
		lerror("bignum generation failed");
		goto free_all;
	}
	
	RSA* rsa_priv_key = NULL;
	
	rsa_priv_key = RSA_new();
	ret = RSA_generate_key_ex(rsa_priv_key, bits, bne, NULL);
	if (ret != 1) {
		lerror("RSA key generation failed");
		goto free_all;
	}
	
	// 3. save private key
	EVP_PKEY_set1_RSA(*target, rsa_priv_key);
	
	// 4. free
	free_all:
	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	BN_free(bne);
	
	return (ret == 1);
}

int x509_generate_write(EncryptServer* parent) {
	if (!rsa_generate(&parent->key_pair)) {
		lerror ("Failed to generate RSA key");
		return 1;
	}
	
	char* certificate_path = server_get_path(parent->parent, "certificate.pem");
	int x509_exists = access(certificate_path, F_OK) != -1;
	if (!x509_exists) {
		linfo("Generating new certificate");
		x509_generate(cert_generate_serial(), 122147281, &parent->certificate, parent->key_pair);
		if (!parent->certificate || !parent->key_pair) {
			free(certificate_path);
			return 2;
		}
	}
	
	FILE* cert_fp;
	FILE* rsa_fp;
	
	int cert_fd = open(certificate_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	if (x509_exists) {
		cert_fp = fdopen(cert_fd, "r");
		parent->certificate = PEM_read_X509(cert_fp, &parent->certificate, NULL, NULL);
	}
	else {
		cert_fp = fdopen(cert_fd, "w");
		
		int x509_status = PEM_write_X509(cert_fp, parent->certificate);
		if (!x509_status) {
			lerror("Failed to write X509 certificate to file");
			fclose(cert_fp);
			remove(certificate_path);
			free(certificate_path);
			return 3;
		}
	}
	
	free(certificate_path);
	fclose (cert_fp);
	
	return 0;
}