
void config_init(void)
{
    config.print = false;
    config.absolute = false;
    config.from_time = MTIME;
    config.to_time = MTIME;
    config.output_unit = SECONDS;
    config.field = '\0'; /* all the time */
}
