void funcs(double x,
           double *p,
           int    np)
;
double expand(double *p, 
              double *a, 
              int    n)
;
int svdfit(double *x,             /* x-data array                              */
       double *y,             /* y-data array                              */
       double *sig,           /* standard deviation array                  */
       int    ndata,          /* number of data points                     */
       double tol,            /* fitting tolerence                         */
       double *a,             /* Returned coefficients                     */
       int    ma,             /* Number of polynomials                     */
       double *u[],           /* U-maxtrix                                 */
       double *v[],           /* V-matrix                                  */
       double *w,             /* W-vector                                  */
       int    mp,             /* Max number of datapoints (array size)     */
       int    np,             /* Max number of polynomials (array size)    */
       double *chisq,         /* Returned value of chi^2                   */
       void   (*Func)(double xf, double *pf, int nf))  /* Function to fit  */
;
int svdvar(double  *v[],
       int     ma,
       double  *w,
       double  *var)
;
int svdcov(double  *v[],
       int     ma,
       double  *w,
       double  *cvm[])
;
