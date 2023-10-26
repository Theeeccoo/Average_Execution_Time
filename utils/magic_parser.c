#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/** DEFINITIONS **/
#define MAX_SIZE 50

/** Prototypes **/
FILE* create_file(char*, const char*);
void  close_file(FILE*);
void  read_file(char*, int, int);

void  fix_fgets(char*);
char* split_line(char*);
float calculate_time(char**, int);
float get_seconds_and_mili(char*);
void  save_final_time(FILE*, float, int);

int main(int argc, char **argv){
	if ( argc < 3 ) {
		perror("ERROR: Invalid number of arguments.\n");
		exit(0);
	}


	char*	path_to_file = argv[1];
	int	iterations_number = atoi(argv[2]),
		number_of_used_threads = atoi(argv[3]);


	if ( iterations_number <= 0 ) {
		perror("ERROR: Number of iterations invalid.\n");
		exit(0);
	}

	if ( number_of_used_threads <= 0 ) {
		perror("ERROR: Number of used threads invalid.\n");
		exit(0);
	}

	read_file(path_to_file, iterations_number, number_of_used_threads);
		
	return 0;
}

/**
 * @brief Magic :)
 *
 * @param path_to_file that contains all executions of "time"
 * @param iterations_number that were executed 
 * @param number_of_used_threads used in the execution of the initial code
 *
 * @details Reads the results and calculate the average time of execution
 */
void read_file(char* path_to_file, int iterations_number, int number_of_used_threads){
	
	FILE	*ptr_write_file = create_file("../result.txt", "w"),
		*ptr_read_file = create_file(path_to_file, "r");

	int 	iterations = iterations_number,
		i = 0;

	char	*buffer = (char*) malloc(sizeof(char) * MAX_SIZE),
		string_to_find[] = "real",
		**auxiliary = (char**) malloc(sizeof(char*) * iterations);

	float	average_execution_time = 0.0f;

	// While there's file to be read
	while ( (fgets(buffer, MAX_SIZE, ptr_read_file)) != NULL ) {
		// Ignores every line that does not contains "real",
		// which is the line that contains the time that we want to calculate the average
		while( strstr(buffer, string_to_find) == NULL && fgets(buffer, MAX_SIZE, ptr_read_file) != NULL );

		// Had to put this verification because it was reading an unwanted line in last iteration
		// Feel free to improve it whenever a better solution is found.
		if( strstr(buffer, string_to_find) != NULL ) {
			fix_fgets(buffer);
			auxiliary[i++] = split_line(buffer);
		}	
	}

	average_execution_time = calculate_time(auxiliary, iterations);
	save_final_time(ptr_write_file, average_execution_time, number_of_used_threads);


	close_file(ptr_write_file);
	close_file(ptr_read_file);
}



/**
 * @brief Creates a new file 
 * 
 * @details Creates a new file pointer based on file path given, if possible
 *
 *
 * @param path_to_file to be created
 * @param mode to open a file (r, w, a, rb wb, a+, w+, r+, x)
 * 
 * @returns FILE pointer
 */
FILE* create_file(char *path_to_file, const char *mode){
	FILE	*ptr_write_file = fopen(path_to_file, mode);
	if ( ptr_write_file == NULL ){
		perror("ERROR: File was not opened correctly\n");
		exit(0);
	}
	
	return ptr_write_file;
}

/**
 * @brief Closes file
 * 
 * @details Closes given file, if exists
 */
void close_file(FILE* ptr_file){
	if ( ptr_file != NULL ) {
		if ( fclose(ptr_file) == 0 ){
			printf("File was closed successfully.\n");
		} else {
			perror("ERROR: File was not closed correctly\n");
			exit(0);
		}
	}
	
}

/**
 * @brief Selects only the string that contains the execution time on line
 *
 * @param buffer that contains the read line from file
 *
 * @details Since every line has a pattern (real    0m0.000ms), we select only the exact substring that 
 * contains the time (in string) of execution from entire string
 *
 * @param char* that contains the reference to the string splitted
 */
char* split_line(char* buffer){

	size_t	buffer_length = strlen(buffer);
	if( buffer_length < 13 ) {
		perror("ERROR: Your string can not be splitted\n");
		exit(0);
	}
	char 	*splitted_string = (char*) malloc(sizeof(char) * 9);
	int	i = 0,
		j = 0;

	for ( i = 5; i < 12; i++, j++ ){
		splitted_string[j] = buffer[i];
	}

	splitted_string[8] = '\0';

	return splitted_string;
}

/**
 * @brief Removing the '\n' that fgets reads
 *
 * @param buffer reference that contains read line
 * @details Receives a pointer to the line that was read and, by reference, fix it,
 * removing the '\n' character and adding the null_terminator '\0'
 */
void fix_fgets(char* buffer){
	size_t	buffer_length = strlen(buffer);
	int	position = buffer_length - 1;
	
	if ( buffer_length > 0 && buffer[position] == '\n' ) {
		buffer[position] = '\0';
	}

}

/**
 * @brief Calculate the average time of execution
 *
 * @param times that contains every splitted line with the times
 * @param iterations contains the number of times that the code was executed 
 *
 * @details Converts the lines and calculate the average time based on 
 * number of iterations
 *
 * @returns average time of execution
 */
float calculate_time(char** times, int iterations){
	int	i = 0,
		controller = 0;

	char	*delimiter = "m",
		*token = NULL;

	float	minutes = 0.0f,
		seconds_and_mili = 0.0f,
		full_time = 0.0f;

	for( ; i < iterations; i++ ){
		token = strtok(times[i], delimiter);
		while ( token != NULL ) {

			// First iteration contains the "minutes" value,
			// Second iteration contains the seconds and miliseconds time,
			// that is why I'm doing those verifications
			if ( controller % 2 == 0 ) {
				minutes = (float) atoi(token) * 60.0f;
			} else  {
				seconds_and_mili = get_seconds_and_mili(token);
				full_time += minutes + seconds_and_mili;

			}

			token = strtok(NULL, delimiter);
			controller ++;
		}
		controller = 0;
	}
	
	// Average time
	full_time /= iterations;
	return full_time;
}

/**
 * @brief Converts the miliseconds string to miliseconds number
 *
 * @param token	that contains the miliseconds string
 *
 * @details Iterates through token and converts every char to a number
 * and puts it in its correct position (using 10th power to find the right position)
 *
 * @returns converted miliseconds number
 */
float get_seconds_and_mili(char* token){
	size_t	token_length = strlen(token);

	int	i = 0,
		j = 0;
	
	float 	time = 0.0f;

	for ( ; i < token_length; i++ ){
		
		if ( token[i] == ',' ) continue;
		char*	auxiliary = (char*) malloc(sizeof(char) * 2);
		auxiliary[0] = token[i];
		auxiliary[1] = '\0';

		time += (float) atoi(auxiliary) / (pow(10, j));
		j++;
	}

	return time;
}

/**
 * @brief Prints the results in the file
 *
 * @param pointer to the file
 * @param average execution time to be printed
 * @param number of threads to be printed 
 */
void save_final_time(FILE* ptr_write_file , float average_execution_time, int num_threads){	
	int	minutes = 0;
	float	seconds_and_milis = 0.0f;

	minutes = average_execution_time / 60;

	// Since average_execution_time is in seconds, if there is no minutes
	// (average_execution_time < 60) it means that average_execution_time = seconds_and_milis
	if ( minutes != 0 ) {
		seconds_and_milis = (minutes * 60) - average_execution_time;
		if ( seconds_and_milis < 0 )
			seconds_and_milis *= (-1);
	} else {
		seconds_and_milis = average_execution_time;
	}


	// Header
	fprintf(ptr_write_file, "%s", "x--- Final Result ---x\n");
	fprintf(ptr_write_file, "%s", "Threads used: ");
	fprintf(ptr_write_file, "%d", num_threads);
	fprintf(ptr_write_file, "%s", "\n");

	// Results
	fprintf(ptr_write_file, "%s", "real    ");
	fprintf(ptr_write_file, "%d", minutes);
	fprintf(ptr_write_file, "%s", "m");
	fprintf(ptr_write_file, "%.3lf", seconds_and_milis);
	fprintf(ptr_write_file, "%s", "s\n");

}
