SELECT sys.pchisq(20.0, 5.0);

SELECT sys.pchisq(20.0, NULL);

SELECT sys.pchisq(NULL, 5.0);

SELECT sys.pchisq(-1, 5.0);

SELECT sys.pchisq(20.0, 1.0);

CREATE TABLE chi2(a double, b double);

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

INSERT INTO chi2 VALUES (20.0, NULL);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (NULL, 5.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (-1, 5.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;

DELETE FROM chi2;

INSERT INTO chi2 VALUES (20.0, 5.0),
       	    	 	(22.0, 4.0),
			(20.0, 6.0),
                        (20.0, 1.0);

SELECT pchisq(a, b) FROM chi2;

SELECT pchisq(a, 6.0) FROM chi2;

SELECT pchisq(19.0, b) FROM chi2;
