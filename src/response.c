#include <autogentoo/response.h>
#include <unistd.h>
#include <asm/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <openssl/ssl.h>

ssize_t rsend(Connection* conn, response_t code) {
	char message[64];
	sprintf(message, "HTTP/1.1 %d %s\r\n", code.code, code.message);
	
	return connection_write(conn, message, strlen(message))
}

response_t res_list[] = {
		OK,
		CREATED,
		NO_CONTENT,
		BAD_REQUEST,
		UNAUTHORIZED,
		FORBIDDEN,
		NOT_FOUND,
		METHOD_NOT_ALLOWED,
		REQUEST_TIMEOUT,
		UPGRADE_REQUIRED,
		INTERNAL_ERROR,
		NOT_IMPLEMENTED,
		BAD_GATEWAY,
		SERVICE_UNAVAILABLE
};

response_t get_res(response_nt x) {
	int i;
	for (i = 0; i != sizeof(res_list) / sizeof(res_list[0]); i++)
		if (res_list[i].code == x)
			return res_list[i];
	return OK;
}