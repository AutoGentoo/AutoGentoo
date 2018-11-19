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

void x509_generate(int serial, int days_valid, X509** cert_out, RSA* key_pair) {
	X509_NAME* name = NULL;
	EVP_PKEY* priv_key = NULL;
	
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
		
		// Now we need a private key object
		priv_key = EVP_PKEY_new();
		if (!priv_key) {
			lerror("Failed to initialize private key");
			break;
		}
		
		// Assign the key pair to the private key object
		if (!EVP_PKEY_assign_RSA (priv_key, key_pair)) {
			lerror("Failed to assign key_pair to private key");
			break;
		}
		
		// Set the certificate's public key from the private key object
		if (!X509_set_pubkey (*cert_out, priv_key)) {
			lerror("Failed to assign private key to certificate");
			break;
		}
		
		// Sign it with SHA-1
		if (!X509_sign (*cert_out, priv_key, EVP_sha256())) {
			lerror("Failed to sign certificate with EVP_sha256()");
			break;
		}
		
		// PrivateKey now belongs to cert_out, so don't clean it up separately
		priv_key = NULL;
		
		status = 1;
	} while (!status);
	
	if (priv_key)
		EVP_PKEY_free (priv_key);
	
	// Things we clean up only on failure
	if (status == 0) {
		X509_free (*cert_out);
		if (priv_key)
			EVP_PKEY_free (priv_key);
		*cert_out = NULL;
	}
}

int cert_generate_serial () {
	return 2;
}

int rsa_generate(RSA** target) {
	int ret = 0;
	BIGNUM* bne = NULL;
	BIO* bp_public = NULL, * bp_private = NULL;
	
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
	
	*target = RSA_new();
	ret = RSA_generate_key_ex(*target, bits, bne, NULL);
	if (ret != 1) {
		lerror("RSA key generation failed");
		goto free_all;
	}
	
	// 3. save private key
	bp_private = BIO_new_file("private.pem", "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, *target, NULL, NULL, 0, NULL, NULL) == 1;
	
	// 4. free
	free_all:
	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	BN_free(bne);
	
	return (ret != 1);
}

int x509_generate_write(EncryptServer* parent) {
	char* certificate_path = server_get_path(parent->parent, "certificate.pem");
	char* rsa_path = server_get_path(parent->parent, "private.pem");
	
	int rsa_exists = access(rsa_path, F_OK) != -1;
	if (!rsa_exists) {
		if (!rsa_generate(&parent->key_pair))
			lerror ("Failed to generate RSA key");
	}
	
	int x509_exists = access(certificate_path, F_OK) != -1;
	if (!x509_exists) {
		linfo("Generating new certificate");
		x509_generate(cert_generate_serial(), 122147281, &parent->certificate, parent->key_pair);
		if (!parent->certificate || !parent->key_pair) {
			free(certificate_path);
			free(rsa_path);
			return 1;
		}
	}
	
	int cert_fd = open(certificate_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	
	int rsa_fd = open(rsa_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
	
	FILE* cert_fp;
	FILE* rsa_fp;
	
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
			free(rsa_path);
			return 2;
		}
	}
	
	if (rsa_exists) {
		rsa_fp = fdopen(rsa_fd, "r");
		parent->key_pair = PEM_read_RSAPrivateKey(rsa_fp, &parent->key_pair, NULL, NULL);
		fclose (rsa_fp);
	}
	
	free(certificate_path);
	free(rsa_path);
	fclose (cert_fp);
	
	return 0;
}