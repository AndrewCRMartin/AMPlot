int process_command(char    *buffer,          /* Command line buffer           */
                DATASET *current_data,    /* Pointer to current dataset    */
                int     *newdata,         /* Flag to show NEWSET found     */
                int     *ndata)           /* Result of NDATA keyword       */
;
int CacheBarlabel(int labnum, char *text)
;
int CacheKeytext(int labnum, char *text)
;
int InitParser(void)
;
