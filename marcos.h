#ifndef _MARCOS_H
#define _MARCOS_H

#define DEFAULT_TCP_PORT 16666
#define MAXEVENTS 1024
#define BUFSIZE 1024
#define FORCE_CLOSE 0
#define MAX_QUEUE_SIZE 51200

/*
 * For worker pool
 */
#define DEFAULT_WORKER_NUM 8

/*
 * For mq.
 */
#define MQ_LOG_PATH "./log/mq.log"
#define MQ_PORT 16667

/*
 * For logger.
 */
#define LOG_LEN 256

/*
 * For config
 */
#define CONFIG_FILE "./config/stage_config.ini"

#endif //
