#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>

#include "castle.h"
#include "castle_public.h"

#include "castle_private.h"

int castle_protocol_version(struct castle_front_connection *conn) {
    struct castle_control_ioctl ctl;
    int ret;
    ctl.cmd = CASTLE_CTRL_PROTOCOL_VERSION;

    ret = ioctl(conn->fd, CASTLE_CTRL_PROTOCOL_VERSION_IOCTL, &ctl);
    if (__builtin_expect(conn->debug_log != NULL, 0)) {
      fprintf(conn->debug_log, "protocol_version() = %d, %d\n", ret, ctl.protocol_version.version);
      fflush(conn->debug_log);
    }
    if (ret)
      return -1;

    return ctl.protocol_version.version;
}

#define C_PRINTF_uint32 "%u"
#define C_PRINTF_uint64 "%lu"
#define C_PRINTF_slave_uuid "%u"
#define C_PRINTF_version "%u"
#define C_PRINTF_size "%zu"
#define C_PRINTF_string "%s"
#define C_PRINTF_collection_id "%u"
#define C_PRINTF_env_var "%u"
#define C_PRINTF_int "%d"
#define C_PRINTF_int32 "%d"

#define CASTLE_IOCTL_0IN_0OUT(_id, _name)                                                         \
int castle_##_id (struct castle_front_connection *conn)                                           \
{                                                                                                 \
    struct castle_control_ioctl ctl;                                                              \
    int ret;                                                                                      \
    ctl.cmd = _name;                                                                              \
                                                                                                  \
    ret = ioctl(conn->fd, _name##_IOCTL, &ctl);                                                   \
    if (__builtin_expect(conn->debug_log != NULL, 0)) {                                           \
      fprintf(conn->debug_log, #_id "() = %d\n", ret);                                            \
      fflush(conn->debug_log);                                                                    \
    }                                                                                             \
    if (ret)                                                                                      \
        return errno;                                                                             \
                                                                                                  \
    return ctl._id.ret;                                                                           \
}

#define CASTLE_IOCTL_1IN_0OUT(_id, _name, _arg_1_t, _arg_1)                                       \
int castle_##_id (struct castle_front_connection *conn,                                           \
    C_TYPE_##_arg_1_t _arg_1)                                                                     \
{                                                                                                 \
    struct castle_control_ioctl ctl;                                                              \
    int ret;                                                                                      \
    ctl.cmd = _name;                                                                              \
    ctl._id._arg_1 = _arg_1;                                                                      \
                                                                                                  \
    ret = ioctl(conn->fd, _name##_IOCTL, &ctl);                                                   \
    if (__builtin_expect(conn->debug_log != NULL, 0)) {                                           \
      fprintf(conn->debug_log,                                                                    \
              #_id "(" #_arg_1 " = " C_PRINTF_##_arg_1_t ") = %d\n",                              \
              _arg_1, ret);                                                                       \
      fflush(conn->debug_log);                                                                    \
    }                                                                                             \
    if (ret)                                                                                      \
        return errno;                                                                             \
                                                                                                  \
    return ctl._id.ret;                                                                           \
}

#define CASTLE_IOCTL_1IN_1OUT(_id, _name, _arg_1_t, _arg_1, _ret_1_t, _ret)                       \
int castle_##_id (struct castle_front_connection *conn,                                           \
    C_TYPE_##_arg_1_t _arg_1,                                                                     \
    C_TYPE_##_ret_1_t * _ret##_out)                                                               \
{                                                                                                 \
    struct castle_control_ioctl ctl;                                                              \
    int ret;                                                                                      \
    ctl.cmd = _name;                                                                              \
    ctl._id._arg_1 = _arg_1;                                                                      \
                                                                                                  \
    ret = ioctl(conn->fd, _name##_IOCTL, &ctl);                                                   \
    if (__builtin_expect(conn->debug_log != NULL, 0)) {                                           \
      fprintf(conn->debug_log, #_id "(" #_arg_1 " = " C_PRINTF_##_arg_1_t                         \
              ", " #_ret " = " C_PRINTF_##_ret_1_t ") = %d\n", _arg_1, ctl._id.ret, ret);         \
      fflush(conn->debug_log);                                                                    \
    }                                                                                             \
    if (ret)                                                                                      \
        return errno;                                                                             \
                                                                                                  \
    * _ret##_out = ctl._id._ret;                                                                  \
                                                                                                  \
    return ctl._id.ret;                                                                           \
}                                                                                                 \

#define CASTLE_IOCTL_2IN_0OUT(_id, _name, _arg_1_t, _arg_1, _arg_2_t, _arg_2)                     \
int castle_##_id (struct castle_front_connection *conn,                                           \
    C_TYPE_##_arg_1_t _arg_1, C_TYPE_##_arg_2_t _arg_2)                                           \
{                                                                                                 \
    struct castle_control_ioctl ctl;                                                              \
    int ret;                                                                                      \
    ctl.cmd = _name;                                                                              \
    ctl._id._arg_1 = _arg_1;                                                                      \
    ctl._id._arg_2 = _arg_2;                                                                      \
                                                                                                  \
    ret = ioctl(conn->fd, _name##_IOCTL, &ctl);                                                   \
    if (__builtin_expect(conn->debug_log != NULL, 0)) {                                           \
      fprintf(conn->debug_log,                                                                    \
              #_id "(" #_arg_1 " = " C_PRINTF_##_arg_1_t                                          \
                  ", " #_arg_2 " = " C_PRINTF_##_arg_2_t ") = %d\n",                              \
              _arg_1, _arg_2, ret);                                                               \
      fflush(conn->debug_log);                                                                    \
    }                                                                                             \
    if (ret)                                                                                      \
        return errno;                                                                             \
                                                                                                  \
    return ctl._id.ret;                                                                           \
}

#define CASTLE_IOCTL_3IN_1OUT(_id, _name, _arg_1_t, _arg_1, _arg_2_t, _arg_2,                     \
    _arg_3_t, _arg_3, _ret_1_t, _ret)                                                             \
int castle_##_id (struct castle_front_connection *conn,                                           \
    C_TYPE_##_arg_1_t _arg_1,                                                                     \
    C_TYPE_##_arg_2_t _arg_2,                                                                     \
    C_TYPE_##_arg_3_t _arg_3,                                                                     \
    C_TYPE_##_ret_1_t * _ret##_out)                                                               \
{                                                                                                 \
    struct castle_control_ioctl ctl;                                                              \
    int ret;                                                                                      \
                                                                                                  \
    ctl.cmd = _name;                                                                              \
    ctl._id._arg_1 = _arg_1;                                                                      \
    ctl._id._arg_2 = _arg_2;                                                                      \
    ctl._id._arg_3 = _arg_3;                                                                      \
                                                                                                  \
    ret = ioctl(conn->fd, _name##_IOCTL, &ctl);                                                   \
    if (__builtin_expect(conn->debug_log != NULL, 0)) {                                           \
      fprintf(conn->debug_log, #_id "(" #_arg_1 " = " C_PRINTF_##_arg_1_t                         \
              ", " #_arg_2 " = " C_PRINTF_##_arg_2_t                                              \
              ", " #_arg_3 " = " C_PRINTF_##_arg_3_t                                              \
              ", " #_ret " = " C_PRINTF_##_ret_1_t ") = %d\n",                                    \
              _arg_1, _arg_2, _arg_3, ctl._id.ret, ret);                                          \
      fflush(conn->debug_log);                                                                    \
    }                                                                                             \
    if (ret)                                                                                      \
        return errno;                                                                             \
                                                                                                  \
    * _ret##_out = ctl._id._ret;                                                                  \
                                                                                                  \
    return ctl._id.ret;                                                                           \
}

CASTLE_IOCTLS
PRIVATE_CASTLE_IOCTLS