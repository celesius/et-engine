LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libcurl

LOCAL_C_INCLUDES = $(LOCAL_PATH)/../../../include

LOCAL_SRC_FILES := file.c timeval.c base64.c hostip.c progress.c formdata.c cookie.c http.c sendf.c ftp.c \
	url.c dict.c if2ip.c speedcheck.c ldap.c ssluse.c version.c getenv.c escape.c mprintf.c \
	telnet.c netrc.c getinfo.c transfer.c strequal.c easy.c security.c krb4.c curl_fnmatch.c \
	fileinfo.c ftplistparser.c wildcard.c krb5.c memdebug.c http_chunks.c strtok.c connect.c \
	llist.c hash.c multi.c content_encoding.c share.c http_digest.c md4.c md5.c curl_rand.c \
	http_negotiate.c inet_pton.c strtoofft.c strerror.c hostasyn.c hostip4.c hostip6.c hostsyn.c \
	inet_ntop.c parsedate.c select.c gtls.c sslgen.c tftp.c splay.c strdup.c socks.c ssh.c nss.c \
	qssl.c rawstr.c curl_addrinfo.c socks_gssapi.c socks_sspi.c curl_sspi.c slist.c nonblock.c \
	curl_memrchr.c imap.c pop3.c smtp.c pingpong.c rtsp.c curl_threads.c warnless.c hmac.c \
	polarssl.c polarssl_threadlock.c curl_rtmp.c openldap.c curl_gethostname.c gopher.c axtls.c \
	http_negotiate_sspi.c cyassl.c http_proxy.c non-ascii.c asyn-ares.c asyn-thread.c \
	curl_gssapi.c curl_ntlm.c curl_ntlm_wb.c curl_ntlm_core.c curl_ntlm_msgs.c curl_sasl.c \
	curl_schannel.c curl_multibyte.c curl_darwinssl.c hostcheck.c bundles.c conncache.c pipeline.c

LOCAL_CFLAGS += -DHAVE_CONFIG_H

include $(BUILD_STATIC_LIBRARY)
