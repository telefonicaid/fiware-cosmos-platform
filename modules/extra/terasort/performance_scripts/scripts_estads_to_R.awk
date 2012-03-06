nawk 'BEGIN { printf("Platform Size Memory Max_File_Size Shm_Size NCores Download Time\n"); }
{
	if (match($1, "plataforma:") > 0)
	{
		nf = split($1, a, ":");
		platform = a[2];
		if (platform == "unix")
		{
			nf = split($2, a, ":");
			size = a[2];
			memory = 20;
			max_file_size = 1;
			shm_size_per_buffer = 256;
			ncores = 1;
			download = "yes";
		}
		else
		{
			nf = split($2, a, ":");
			size = a[2];
			nf = split($3, a, ":");
			memory = a[2];
			nf = split($4, a, ":");
			max_file_size = a[2];
			nf = split($5, a, ":");
			shm_size_per_buffer = a[2];
			nf = split($6, a, ":");
			ncores = a[2];
			nf = split($7, a, ":");
			download = a[2];
		}
		hay_datos = 1;
	}
	else if (match($1, "Command") > 0)
	{
		hay_datos = 0;
	}
	else if (match($1, "real") > 0)
	{
		if (hay_datos == 1)
		{
			printf("%s %d %d %.2f %d %d %s %.2f\n", platform, size, memory, max_file_size, shm_size_per_buffer, ncores, download, $2);
			hay_datos = 0;
		}
	}
}' $1
