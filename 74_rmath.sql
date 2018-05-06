-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0.  If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 1997 - July 2008 CWI, August 2008 - 2017 MonetDB B.V.

-- (co) Arjen de Rijke, Bart Scheers
-- Use statistical functions from rmath library

-- Calculate Chi squared probability
create function sys.pchisq(q double, df double, lower integer, log integer)
returns double external name rmath."pchisq";

create function sys.pchisq(q double, df double)
returns double begin return pchisq(q, df, 1, 0); end;

create function sys.qchisq(p double, df double, lower integer, log integer)
returns double external name rmath."qchisq";

create function sys.qchisq(p double, df double)
returns double begin return qchisq(p, df, 1, 0); end;

create function sys.dchisq(x double, df double, log integer)
returns double external name rmath."dchisq";

create function sys.dchisq(x double, df double)
returns double begin return dchisq(x, df, 0); end;


-- Calculate normal CDF
create function sys.pnorm(q double, mean double, sd double, lower integer, log integer)
returns double external name rmath."pnorm";

create function sys.pnorm(q double, mean double, sd double)
returns double begin return pnorm(q, mean, sd, 1, 0); end;

create function sys.qnorm(p double, mean double, sd double, lower integer, log integer)
returns double external name rmath."qnorm";

create function sys.qnorm(p double, mean double, sd double)
returns double begin return qnorm(p, mean, sd, 1, 0); end;

create function sys.dnorm(x double, mean double, sd double, log integer)
returns double external name rmath."dnorm";

create function sys.dnorm(x double, mean double, sd double)
returns double begin return dnorm(x, mean, sd, 0); end;

-- Gamma distribution
create function sys.pgamma(q double, shape double, scale double, lower integer, log integer)
returns double external name rmath."pgamma";

create function sys.pgamma(q double, shape double, scale double)
returns double begin return pgamma(q, shape, scale, 1, 0); end;

create function sys.qgamma(p double, shape double, scale double, lower integer, log integer)
returns double external name rmath."qgamma";

create function sys.qgamma(p double, shape double, scale double)
returns double begin return qgamma(p, shape, scale, 1, 0); end;

create function sys.dgamma(x double, shape double, scale double, log integer)
returns double external name rmath."dgamma";

create function sys.dgamma(x double, shape double, scale double)
returns double begin return dgamma(x, shape, scale, 0); end;


create function sys.poissonci(x integer, idx integer, conflevel double)
       returns double
       begin
       declare alpha double;
       set alpha = (1.0-conflevel)/2.0;
       if (x=0 and idx=1) then return 0.0;
       elseif (idx=1) then return qgamma(alpha, cast(x as double), 1.0);
       else return qgamma(1.0-alpha,cast(1+x as double),1.0);
       end if;
end;
create function sys.poissonci(x integer, idx integer)
returns double begin return poissonci(x,idx,0.95); end;
