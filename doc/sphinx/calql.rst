The Caliper Query Language
================================

The Caliper Query Language (CalQL) is used to filter, aggregate, and
create reports from Caliper data in Caliper's data processing tools
(cali-query) and report services. Its syntax is very similar to SQL.

For example, filtering and aggregating a trace into a function time
profile for an application's main loop and printing results in a
table, sorted by time, is accomplished with the following statement:
::

  SELECT *, count(), sum(time.duration) 
  WHERE loop=mainloop 
  GROUP BY function 
  FORMAT table 
  ORDER BY time.duration

SELECT
--------------------------------

The SELECT statement selects which attributes in snapshot records
should be printed, and defines aggregations. ``SELECT *`` selects all
attributes. ``SELECT op(arg)`` enables aggregation operation `op` with
argument `arg`. Generally, attributes will be printed in the order
given in the SELECT statement.

Example::

  SELECT *, count()

Print all attributes and enable visit count aggregation.

::

  SELECT function, annotation, sum(time.duration)

Aggregate `time.duration` using `sum` and print `function` and
`annotation` attributes from snapshot records.

WHERE
--------------------------------

The WHERE statement can be used to filter the records to aggregate/print.
The statement takes a comma-separated list of clauses. Records that don't
match all of the clauses are filtered out.

Currently, there are clauses to test for existance of an attribute
label in a record, and to filter for specific attribute
values. Clauses can be negated with ``NOT``. Example: ::

  WHERE loop,function=foo

Select records that contain ``loop`` attributes where function equals
``foo``. Note: for nested attributes, the match may occur on any
level. The above example matches the following::

  { loop=mainloop, iteration=5, function=main/foo }      Matches
  { loop=mainloop, iteration=5, function=main/foo/bar }  Matches ('function=foo' will be matched on any nesting level)
  { function=main/foo }                                  No match: 'loop' attribute missing

The ``NOT`` keyword negates clauses: ::

  WHERE NOT iteration#mainloop=4

matches every record where ``iteration#mainloop`` is not 4 (including
records without ``iteration#mainloop`` attributes.

GROUP BY
--------------------------------

The GROUP BY statement defines the `aggregation key` for aggregation
operations. The aggregation key defines for which attributes separate
(aggregate) records will be kept. That is, the aggregator will
generate an aggregate record for each unique combination of key values
found in the input records.  The values of the aggregation attributes
in the input records will be accumulated and appended to the aggregate
record.

Consider the following table of input records::

  function loop     iteration
           mainloop
  foo      mainloop         0
  bar      mainloop         0
  foo      mainloop         1
  bar      mainloop         1
  foo      mainloop         2
  bar      mainloop         2
  

With this input, the following GROUP BY statement will create a
function profile::

  SELECT *, count() GROUP BY function

  function aggregate.count
  foo                    3
  bar                    3

FORMAT
--------------------------------

The FORMAT statement selects the output format option. Caliper can
produce machine-readable (e.g., json or Caliper's own csv-style) or
human-readable output (text tables or a tree representation).

Currently available formatters are

* csv (Caliper's default machine-readable format)
* json 
* expand (expanded attr1=value1,attr2=value2,... records)
* table (human-readable text table)
* tree (print records in a tree based on the hierarchy of selected path attributes)

ORDER BY
--------------------------------

Sort output records by the given sort criteria. The statement defines
a list of attributes to sort output records by. Records can be sorted
ascending (using the ASC keyword) or descending (using DESC). Note
that the sorting is performed by the output formatter and only
available in some formatters (e.g., table).

The following example prints a iteration/function profile ordered by
time and iteration number: ::

  SELECT *, sum(time.duration) FORMAT table ORDER BY time.inclusive.duration DESC, iteration#mainloop

  function loop     iteration#mainloop time.inclusive.duration
  main                                                  100000
  main     mainloop                                      80000
  main/foo mainloop                  0                    2500
  main     mainloop                  0                    1500
  main/foo mainloop                  1                    3500
  main     mainloop                  1                    2000
  main     mainloop                  2                    1000
  main/foo mainloop                  2                     600
  ...
