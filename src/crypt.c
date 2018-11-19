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

void x509_generate(int serial, int days_valid, X509** cert_out, RSA** key_pair) {
	X509_NAME* name = NULL;
	BIGNUM* bne = NULL;
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
		
		// Create the RSA key pair object
		*key_pair = RSA_new();
		if (!(*key_pair)) {
			lerror("Failed to initialize key pair");
			break;
		}
		
		// Create the big number object
		bne = BN_new();
		if (!bne) {
			lerror("Failed to generate big number");
			break;
		}
		
		// Set the word
		if (!BN_set_word (bne, 65537)) {
			lerror("Failed to generate big number word");
			break;
		}
		
		// Generate the key pair; lots of computes here
		if (!RSA_generate_key_ex (*key_pair , AUTOGENTOO_RSA_BITS, bne, NULL)) {
			lerror("Failed to generate key pair");
			break;
		}
		
		// Now we need a private key object
		priv_key = EVP_PKEY_new();
		if (!priv_key) {
			lerror("Failed to initialize private key");
			break;
		}
		
		// Assign the key pair to the private key object
		if (!EVP_PKEY_assign_RSA (priv_key, *key_pair)) {
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
	
	// Things we always clean up
	if (bne)
		BN_free (bne);
	if (priv_key)
		EVP_PKEY_free (priv_key);
	
	// Things we clean up only on failure
	if (status == 0) {
		X509_free (*cert_out);
		if (priv_key)
			EVP_PKEY_free (priv_key);
		if (key_pair)
			RSA_free (*key_pair);
		*cert_out = NULL;
	}
}

int cert_generate_serial () {
	return 2;
}

int x509_generate_write(EncryptServer* parent) {
	char* certificate_path = server_get_path(parent->parent, "certificate.pem");
	char* rsa_path = server_get_path(parent->parent, "private.pem");
	
	int exists = access(certificate_path, F_OK) != -1 && access(rsa_path, F_OK) != -1;
	if (!exists) {
		linfo("Generating new certificate");
		x509_generate(cert_generate_serial(), 122147281, &parent->certificate, &parent->key_pair);
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
	
	if (exists) {
		cert_fp = fdopen(cert_fd, "r");
		rsa_fp = fdopen(rsa_fd, "r");
		parent->certificate = PEM_read_X509(cert_fp, &parent->certificate, NULL, NULL);
		parent->key_pair = PEM_read_RSAPrivateKey(rsa_fp, &parent->key_pair, NULL, NULL);
	}
	else {
		cert_fp = fdopen(cert_fd, "w");
		rsa_fp = fdopen(rsa_fd, "w");
		
		int x509_status = PEM_write_X509(cert_fp, parent->certificate);
		int rsa_status = PEM_write_RSAPrivateKey(rsa_fp, parent->key_pair, NULL, NULL, 0, NULL, NULL);
		if (!x509_status)
			lerror ("Failed to write X509 certificate to file");
		if (!rsa_status)
			lerror ("Failed to write RSA private key to file");
		if (!rsa_status || !x509_status) {
			fclose (cert_fp);
			fclose (rsa_fp);
			
			remove (certificate_path);
			remove (rsa_path);
			free(certificate_path);
			free(rsa_path);
			return 2;
		}
	}
	
	free(certificate_path);
	free(rsa_path);
	fclose (cert_fp);
	fclose (rsa_fp);
	
	return 0;
}