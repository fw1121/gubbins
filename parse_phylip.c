/*
 *  Wellcome Trust Sanger Institute
 *  Copyright (C) 2011  Wellcome Trust Sanger Institute
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_phylip.h"
#include "alignment_file.h"

int num_samples;
int num_snps;
char ** sequences;
char ** sample_names;


void load_sequences_from_phylib_file(char phylip_filename[])
{
	FILE * phylib_file_pointer;
	phylib_file_pointer=fopen(phylip_filename, "r");
	load_sequences_from_phylib(phylib_file_pointer);
}

void load_sequences_from_phylib(FILE * phylip_file_pointer)
{	
	rewind(phylip_file_pointer);
	char line_buffer[MAX_READ_BUFFER] = {0}; 
	int i;
	
	// The first line contains the number of samples and snps
	strcpy(line_buffer,"");
	read_line(line_buffer, phylip_file_pointer);
	
	num_samples = get_number_of_samples_from_phylip(line_buffer);
	num_snps = get_number_of_snps_from_phylip(line_buffer);
	

	sequences    = (char **) malloc(num_samples*sizeof(char));
	sample_names = (char **) malloc(num_samples*sizeof(char));
	for(i = 0; i < num_samples; i++)
	{
		sequences[i] = (char *) malloc(num_snps*sizeof(char));
		sample_names[i] = (char *) malloc(MAX_SAMPLE_NAME_SIZE*sizeof(char));
	}
	
	int sample_counter = 0;

	do{
		strcpy(line_buffer,""); 
		read_line(line_buffer, phylip_file_pointer);
		
		if(line_buffer[0] == '\0')
		{
			break;
		}
		
		int found_sequence = 0;
		int sequence_offset = 0;
		for(i = 0 ; i< num_snps; i++)
		{
			if(line_buffer[i] == '\0' || line_buffer[i] == '\n')
			{
				sequences[sample_counter][i-sequence_offset] = '\0';
				break;	
			}
			else if(found_sequence == 1)
			{
				// Read in the sequence data of the sample
				sequences[sample_counter][i-sequence_offset] = line_buffer[i];
			}
			else
			{
				// Read in the name of the sample
				if(line_buffer[i] == '\t')
				{
					found_sequence = 1;
					sequence_offset = i+1;
					sample_names[sample_counter][i] = '\0';
				}
				else
				{
					sample_names[sample_counter][i] = line_buffer[i];
				}
			}
		}
		printf("%s\n",sample_names[sample_counter]);
		
		sample_counter++;
		
	}while(line_buffer[0] != '\0');

}

int get_number_of_samples_from_phylip(char * phylip_string)
{
	int i;
	char number_of_samples_string[20] = {0}; 

	for(i = 0; i< MAX_READ_BUFFER; i++)
	{
		if(phylip_string[i] == '\0' || phylip_string[i] == '\n' || phylip_string[i] == ' ')
		{
			break;	
		}
		else
		{
			number_of_samples_string[i] = phylip_string[i];
		}
	}
	
	return atoi(number_of_samples_string);
}

int get_number_of_snps_from_phylip(char * phylip_string)
{
	int found = 0;
	int i;
	int offset = 0;
	char number_of_snps_string[20] = {0}; 
	
	for(i = 0; i< MAX_READ_BUFFER; i++)
	{
		if(phylip_string[i] == '\0' || phylip_string[i] == '\n' )
		{
			break;
		}
		
		if(phylip_string[i] == ' ')
		{
			found = 1;
			offset = i+1;
		}
		else
		{
			if(found == 1)
			{
				number_of_snps_string[i-offset] = phylip_string[i];
			}
		}
	}
	
	return atoi(number_of_snps_string);
}