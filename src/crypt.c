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
	
	int status = 0;
	
	const unsigned char* country = (unsigned char*)"United States of America";
	const unsigned char* organization = (unsigned char*)"AutoGentoo";
	const unsigned char* common = (unsigned char*)"autogentoo";
	
	// Create the certificate object
	*cert_out = X509_new();
	if (!*cert_out) {
		X509_free (*cert_out);
		lerror ("Failed to initialize certificate");
		*cert_out = NULL;
		return;
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
	
	certificate_sign(*cert_out, key_pair);
}

int certificate_sign(X509* cert, RSA* rsa) {
	int status = 0;
	EVP_PKEY* priv_key;
	
	do {
		// Create the certificate object
		priv_key = EVP_PKEY_new();
		if (!priv_key) {
			lerror("Failed to initialize EVP_PKEY");
			break;
		}
		
		if (!EVP_PKEY_assign_RSA(priv_key, rsa)) {
			lerror("Failed to assign key_pair to private key");
			break;
		}
		
		// Set the certificate's public key from the private key object
		if (!X509_set_pubkey (cert, priv_key)) {
			lerror("Failed to assign private key to certificate");
			break;
		}
		
		// Sign it with SHA-1
		if (!X509_sign (cert, priv_key, EVP_sha256())) {
			lerror("Failed to sign certificate with EVP_sha256()");
			break;
		}
		
		priv_key = NULL;
		
		status = 1;
	} while (!status);
	
	EVP_PKEY_free(priv_key);
	
	return status;
}

int cert_generate_serial () {
	return 2;
}

int rsa_generate(RSA** target) {
	int ret = 0;
	BIGNUM* bne = NULL;
	
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
	
	// 4. free
	free_all:
	BN_free(bne);
	
	return (ret == 1);
}

int x509_generate_write(EncryptServer* parent) {
	if (parent->opts & ENC_READ_RSA && parent->opts & ENC_GEN_RSA) {
		lerror("invalid opts: Can't generate and read RSA");
		return 1;
	}
	if (parent->opts & ENC_READ_CERT && parent->opts & ENC_GEN_CERT) {
		lerror("invalid opts: Can't generate and read certificate");
		return 1;
	}
	
	if (parent->opts & ENC_READ_RSA) {
		FILE* fp = fopen(parent->rsa_path, "r");
		parent->key_pair = PEM_read_RSAPrivateKey(fp, &parent->key_pair, NULL, NULL);
		fclose(fp);
		if (!parent->key_pair) {
			lerror("Failed to read RSA from file %s", parent->rsa_path);
			return 2;
		}
	}
	
	if (parent->opts & ENC_READ_CERT) {
		FILE* fp = fopen(parent->cert_path, "r");
		parent->certificate = PEM_read_X509(fp, &parent->certificate, NULL, NULL);
		fclose(fp);
		if (!parent->certificate) {
			lerror("Failed to read certificate from file %s", parent->cert_path);
			return 3;
		}
	}
	
	if (parent->opts & ENC_GEN_RSA) {
		if (!rsa_generate(&parent->key_pair)) {
			lerror ("Failed to generate RSA key");
			return 4;
		}
		
		int rsa_fd = open(parent->rsa_path, O_RDWR | O_CREAT, 0 | S_IRUSR | S_IWUSR | S_IRGRP);
		FILE* fp = fdopen(rsa_fd, "w");
		if (!PEM_write_RSAPrivateKey(fp, parent->key_pair, NULL, NULL, 0, NULL, NULL)) {
			fclose (fp);
			lerror("Failed to write private key to file");
			return 4;
		}
		fclose (fp);
	}
	
	if (parent->opts & ENC_GEN_CERT) {
		x509_generate(cert_generate_serial(), 120, &parent->certificate, parent->key_pair);
		if (!parent->certificate) {
			lerror("Failed to generate certificate");
			return 5;
		}
		
		int cert_fd = open(parent->cert_path, O_RDWR | O_CREAT, 0 | S_IRUSR | S_IWUSR | S_IRGRP);
		FILE* fp = fdopen(cert_fd, "w");
		if (!PEM_write_X509(fp, parent->certificate)) {
			fclose (fp);
			lerror("Failed to write certificate to file");
			return 5;
		}
		fclose (fp);
	}
	
	if (parent->opts & ENC_CERT_SIGN) {
		if (!certificate_sign(parent->certificate, parent->key_pair)) {
			lerror("Failed to sign RSA key with certificate");
			return 6;
		}
	}
	
	return 0;
}