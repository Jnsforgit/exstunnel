#include "common/common.h"
#include "core/core.h"

static int s_debug_level = EST_LL_INFO;
static const char *s_root_dir = ".";
static const char *s_listening_address = "http://0.0.0.0:8000";
static const char *s_enable_hexdump = "no";
static const char *s_ssi_pattern = "#.html";

// Handle interrupts, like Ctrl-C
static int s_signo;

static void cb(Connection_t *c, int ev, void *ev_data, void *fn_data)
{
    (void)fn_data;
}

static void signal_handler(int signo)
{
    s_signo = signo;
}

int parseCmdline(int argc, char **argv)
{
    int i = 0;

    /* Parse command-line flags */
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            s_root_dir = argv[++i];
        }
        else if (strcmp(argv[i], "-H") == 0)
        {
            s_enable_hexdump = argv[++i];
        }
        else if (strcmp(argv[i], "-S") == 0)
        {
            s_ssi_pattern = argv[++i];
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            s_listening_address = argv[++i];
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            s_debug_level = atoi(argv[++i]);
        }
        else
        {
            usage(argv[0]);
        }
    }
}

int main(int argc, char *argv[])
{
    Connection_t *conn = NULL;
    Context_t ctx;
    char path[BUFF_SIZE_FILENAME] = ".";

    parseCmdline(argc, argv);

    // Root directory must not contain double dots. Make it absolute
    // Do the conversion only if the root dir spec does not contain overrides
    if (strchr(s_root_dir, ',') == NULL)
    {
        realpath(s_root_dir, path);
        s_root_dir = path;
    }

    // Initialise stuff
#if (EST_ARCH == EST_ARCH_UNIX)
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif

    est_log_set(s_debug_level);
    contextInit(&ctx);
    if ((conn = mg_http_listen(&ctx, s_listening_address, cb, &ctx)) == NULL)
    {
        LOGE(("Cannot listen on %s. Use http://ADDR:PORT or :PORT", s_listening_address));
        exit(EXIT_FAILURE);
    }

    if (est_casecmp(s_enable_hexdump, "yes") == 0)
    {
        conn->is_hexdumping = 1;
    }

    /* Start infinite event loop */
    LOGI(("Mongoose version : v%s", EST_VERSION));
    LOGI(("Listening on     : %s", s_listening_address));
    LOGI(("Web root         : [%s]", s_root_dir));
    while (s_signo == 0)
    {
        mg_mgr_poll(&ctx, 1000);
    }

    mg_mgr_free(&ctx);
    LOGI(("Exiting on signal %d", s_signo));

    return 0;
}