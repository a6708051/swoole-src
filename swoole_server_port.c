/*
  +----------------------------------------------------------------------+
  | Swoole                                                               |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | license@swoole.com so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
 */

#include "php_swoole.h"

zend_class_entry swoole_server_port_ce;
zend_class_entry *swoole_server_port_class_entry_ptr;

static PHP_METHOD(swoole_server_port, __destruct);
static PHP_METHOD(swoole_server_port, on);
static PHP_METHOD(swoole_server_port, set);

const zend_function_entry swoole_server_port_methods[] =
{
    PHP_ME(swoole_server_port, __destruct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(swoole_server_port, set,             NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swoole_server_port, on,              NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void swoole_server_port_init(int module_number TSRMLS_DC)
{
    INIT_CLASS_ENTRY(swoole_server_port_ce, "swoole_server_port", swoole_server_port_methods);
    swoole_server_port_class_entry_ptr = zend_register_internal_class(&swoole_server_port_ce TSRMLS_CC);
}

static PHP_METHOD(swoole_server_port, __destruct)
{
    swoole_port_callbacks *callbacks = swoole_get_property(getThis(), 0);
    efree(callbacks);
    swoole_set_property(getThis(), 0, NULL);
    swoole_set_object(getThis(), NULL);
}

static PHP_METHOD(swoole_server_port, set)
{
    zval *zset = NULL;
    HashTable *vht;
    zval *v;

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "z", &zset) == FAILURE)
    {
        return;
    }

    vht = Z_ARRVAL_P(zset);
    swListenPort *port = swoole_get_object(getThis());
    if (port == NULL)
    {
        swoole_php_fatal_error(E_ERROR, "Please use the swoole_server->listen method.");
        return;
    }

    //backlog
    if (sw_zend_hash_find(vht, ZEND_STRS("backlog"), (void **) &v) == SUCCESS)
    {
        convert_to_long(v);
        port->backlog = (int) Z_LVAL_P(v);
    }
    //tcp_nodelay
    if (sw_zend_hash_find(vht, ZEND_STRS("open_tcp_nodelay"), (void **) &v) == SUCCESS)
    {
        convert_to_boolean(v);
        port->open_tcp_nodelay = Z_BVAL_P(v);
    }
    //tcp_defer_accept
    if (sw_zend_hash_find(vht, ZEND_STRS("tcp_defer_accept"), (void **) &v) == SUCCESS)
    {
        convert_to_long(v);
        port->tcp_defer_accept = (uint8_t) Z_LVAL_P(v);
    }
   //port reuse
   if (sw_zend_hash_find(vht, ZEND_STRS("enable_port_reuse"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       SwooleG.reuse_port = Z_BVAL_P(v);
   }
   //tcp_keepalive
   if (sw_zend_hash_find(vht, ZEND_STRS("open_tcp_keepalive"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       port->open_tcp_keepalive = Z_BVAL_P(v);
   }
   //buffer: split package with eof
   if (sw_zend_hash_find(vht, ZEND_STRS("open_eof_split"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       port->protocol.split_by_eof = Z_BVAL_P(v);
       port->open_eof_check = 1;
   }
   //package eof
   if (sw_zend_hash_find(vht, ZEND_STRS("package_eof"), (void **) &v) == SUCCESS)
   {
       convert_to_string(v);
       port->protocol.package_eof_len = Z_STRLEN_P(v);
       if (port->protocol.package_eof_len > SW_DATA_EOF_MAXLEN)
       {
           php_error_docref(NULL TSRMLS_CC, E_ERROR, "pacakge_eof max length is %d", SW_DATA_EOF_MAXLEN);
           RETURN_FALSE;
       }
       bzero(port->protocol.package_eof, SW_DATA_EOF_MAXLEN);
       memcpy(port->protocol.package_eof, Z_STRVAL_P(v), Z_STRLEN_P(v));
   }
   //buffer: http_protocol
   if (sw_zend_hash_find(vht, ZEND_STRS("open_http_protocol"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       port->open_http_protocol = Z_BVAL_P(v);
   }
   //buffer: mqtt protocol
   if (sw_zend_hash_find(vht, ZEND_STRS("open_mqtt_protocol"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       port->open_mqtt_protocol = Z_BVAL_P(v);
   }
   //tcp_keepidle
   if (sw_zend_hash_find(vht, ZEND_STRS("tcp_keepidle"), (void **) &v) == SUCCESS)
   {
       convert_to_long(v);
       port->tcp_keepidle = (uint16_t) Z_LVAL_P(v);
   }
   //tcp_keepinterval
   if (sw_zend_hash_find(vht, ZEND_STRS("tcp_keepinterval"), (void **) &v) == SUCCESS)
   {
       convert_to_long(v);
       port->tcp_keepinterval = (uint16_t) Z_LVAL_P(v);
   }
   //tcp_keepcount
   if (sw_zend_hash_find(vht, ZEND_STRS("tcp_keepcount"), (void **) &v) == SUCCESS)
   {
       convert_to_long(v);
       port->tcp_keepcount = (uint16_t) Z_LVAL_P(v);
   }
   //open length check
   if (sw_zend_hash_find(vht, ZEND_STRS("open_length_check"), (void **) &v) == SUCCESS)
   {
       convert_to_boolean(v);
       port->open_length_check = Z_BVAL_P(v);
   }
   //package length size
   if (sw_zend_hash_find(vht, ZEND_STRS("package_length_type"), (void **)&v) == SUCCESS)
   {
       convert_to_string(v);
       port->protocol.package_length_type = Z_STRVAL_P(v)[0];
       port->protocol.package_length_size = swoole_type_size(port->protocol.package_length_type);

       if (port->protocol.package_length_size == 0)
       {
           php_error_docref(NULL TSRMLS_CC, E_ERROR, "unknow package_length_type, see pack(). Link: http://php.net/pack");
           RETURN_FALSE;
       }
   }
   //package length offset
   if (sw_zend_hash_find(vht, ZEND_STRS("package_length_offset"), (void **)&v) == SUCCESS)
   {
       convert_to_long(v);
       port->protocol.package_length_offset = (int)Z_LVAL_P(v);
   }
   //package body start
   if (sw_zend_hash_find(vht, ZEND_STRS("package_body_offset"), (void **) &v) == SUCCESS
           || sw_zend_hash_find(vht, ZEND_STRS("package_body_start"), (void **) &v) == SUCCESS)
   {
       convert_to_long(v);
       port->protocol.package_body_offset = (int) Z_LVAL_P(v);
   }
   /**
    * package max length
    */
   if (sw_zend_hash_find(vht, ZEND_STRS("package_max_length"), (void **) &v) == SUCCESS)
   {
       convert_to_long(v);
       port->protocol.package_max_length = (int) Z_LVAL_P(v);
   }
    /**
     * swoole_packet_mode
     */
    if (SwooleG.serv->packet_mode == 1)
    {
        port->protocol.package_max_length = 64 * 1024 * 1024;
        port->open_length_check = 1;
        port->protocol.package_length_offset = 0;
        port->protocol.package_body_offset = 4;
        port->protocol.package_length_type = 'N';
        port->open_eof_check = 0;
    }

#ifdef SW_USE_OPENSSL
    if (sw_zend_hash_find(vht, ZEND_STRS("ssl_cert_file"), (void **) &v) == SUCCESS)
    {
        convert_to_string(v);
        if (access(Z_STRVAL_P(v), R_OK) < 0)
        {
            swoole_php_fatal_error(E_ERROR, "ssl cert file[%s] not found.", Z_STRVAL_P(v));
            return;
        }
        port->ssl_cert_file = strdup(Z_STRVAL_P(v));
        port->open_ssl_encrypt = 1;
    }
    if (sw_zend_hash_find(vht, ZEND_STRS("ssl_key_file"), (void **) &v) == SUCCESS)
    {
        convert_to_string(v);
        if (access(Z_STRVAL_P(v), R_OK) < 0)
        {
            swoole_php_fatal_error(E_ERROR, "ssl key file[%s] not found.", Z_STRVAL_P(v));
            return;
        }
        port->ssl_key_file = strdup(Z_STRVAL_P(v));
    }
    if (sw_zend_hash_find(vht, ZEND_STRS("ssl_method"), (void **) &v) == SUCCESS)
    {
        convert_to_long(v);
        port->ssl_method = (int) Z_LVAL_P(v);
    }
    //verify client cert
    if (sw_zend_hash_find(vht, ZEND_STRS("ssl_client_cert_file"), (void **) &v) == SUCCESS)
    {
        convert_to_string(v);
        if (access(Z_STRVAL_P(v), R_OK) < 0)
        {
            swoole_php_fatal_error(E_ERROR, "ssl cert file[%s] not found.", port->ssl_cert_file);
            return;
        }
        port->ssl_client_cert_file = strdup(Z_STRVAL_P(v));
    }
    if (sw_zend_hash_find(vht, ZEND_STRS("ssl_verify_depth"), (void **) &v) == SUCCESS)
    {
        convert_to_long(v);
        port->ssl_verify_depth = (int) Z_LVAL_P(v);
    }
    if (port->open_ssl_encrypt && !port->ssl_key_file)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "ssl require key file.");
        return;
    }
#endif
}

static PHP_METHOD(swoole_server_port, on)
{
    char *ha_name = NULL;
    zend_size_t len, i;
    int ret = -1;

    zval *cb;

    if (SwooleGS->start > 0)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Server is running. Unable to set event callback now.");
        RETURN_FALSE;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS()TSRMLS_CC, "sz", &ha_name, &len, &cb) == FAILURE)
    {
        return;
    }

    swoole_port_callbacks *callbacks = swoole_get_property(getThis(), 0);

    swListenPort *port = swoole_get_object(getThis());
    if (!port->ptr)
    {
        port->ptr = callbacks;
    }

    char *callback[PHP_SERVER_PORT_CALLBACK_NUM] = {
        "connect",
        "receive",
        "close",
        "packet",
    };

    for (i = 0; i < PHP_SERVER_PORT_CALLBACK_NUM; i++)
    {
        if (strncasecmp(callback[i], ha_name, len) == 0)
        {
            ret = php_swoole_set_callback(callbacks->array, i, cb TSRMLS_CC);
            if (i == SW_SERVER_CB_onConnect && SwooleG.serv->onConnect == NULL)
            {
                SwooleG.serv->onConnect = php_swoole_onConnect;
            }
            else if (i == SW_SERVER_CB_onClose && SwooleG.serv->onClose == NULL)
            {
                SwooleG.serv->onClose = php_swoole_onClose;
            }
            break;
        }
    }
    if (ret < 0)
    {
        swoole_php_error(E_WARNING, "Unknown event types[%s]", ha_name);
    }
    SW_CHECK_RETURN(ret);
}
