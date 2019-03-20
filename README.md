# impala-levenshtein

Stand-alone version of Impala `levenshtein()` function per [IMPALA-7759](https://issues.apache.org/jira/browse/IMPALA-7759)

Read:
- https://blog.cloudera.com/blog/2014/01/how-to-get-started-writing-impala-udfs/
- https://impala.apache.org/docs/build/html/topics/impala_udf.html#udf_demo_env

Verify Impala UDF SDK is installed:

```
$ rpm -qa | grep impala-udf-devel
impala-udf-devel-3.1.0+cdh6.1.1-875250.el7.x86_64
```

Install if not. Find the right rpm (example for C5.16 on RedHat 6) and install like such:

```
sudo yum install http://archive.cloudera.com/cdh5/redhat/6/x86_64/cdh/5.15.0/RPMS/x86_64/impala-udf-devel-2.12.0+cdh5.15.0+0-1.cdh5.15.0.p0.52.el6.x86_64.rpm
```

Install necessary dev tools:

```
sudo yum install gcc-c++ cmake boost-devel
```

Clone and build:

```
git clone https://github.com/gregrahn/impala-levenshtein.git
cd impala-levenshtein
cmake .
make
hdfs dfs -put -f build/libudf-levenshtein.so /tmp
```

Create and test function in Impala:

```sql
create function levenshtein(string, string) returns int
location '/tmp/libudf-levenshtein.so' symbol='Levenshtein';

+----------------------------+
| summary                    |
+----------------------------+
| Function has been created. |
+----------------------------+

select levenshtein('honda','hyundai') as edit_distance;

+---------------+
| edit_distance |
+---------------+
| 3             |
+---------------+
```
