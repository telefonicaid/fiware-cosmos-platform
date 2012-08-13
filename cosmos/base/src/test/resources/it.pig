/*
 * This script supports integration test between Cosmos and Apache Pig. It
 * implements a simple join between workers and their deparments.
 *
 * It expects the following parameters.
 *    workers_file     : the file that contains the CSV for workers
 *    departments_file : the file that contains the CSV for departments
 *    output_dir       : the directory where results are written
 */

workers = LOAD '$workers_file' USING PigStorage(',')
    AS (worker_id:int, worker_name:chararray, department_id:int);

departments = LOAD '$departments_file' USING PigStorage(',')
    AS (department_id:int, department_name:chararray);

result = JOIN workers BY department_id, departments BY department_id;

STORE result INTO '$output_dir' USING PigStorage(',');
