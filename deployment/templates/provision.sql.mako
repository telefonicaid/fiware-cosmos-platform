CREATE DATABASE IF NOT EXISTS hue;
GRANT USAGE ON *.* TO 'hue'@'localhost' IDENTIFIED BY '${hue_db_pwd}';
GRANT ALL PRIVILEGES ON hue.* TO 'hue'@'localhost';
