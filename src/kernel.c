/*
 * kernel.c
 * 
 * Copyright 2017 Unknown <atuser@Hyperion>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <hash.h>
#include <request.h>
#include <sys/mman.h>
#include <kernel.h>
#include <response.h>
#include <unistd.h>

struct kernel_client* init_kernel (struct manager* m_man, int sc_no, char* architecture) {
    struct kernel_client* out_k = mmap(NULL, sizeof (struct kernel_client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    pid_t k_info_pid = fork();
    if (k_info_pid == 0) {
        chdir ("/usr/src/linux");
        FILE* fd_krelease = popen("make -s kernelversion 2> /dev/null", "r");
        fgets(out_k->release, 32, fd_krelease);
        pclose (fd_krelease);
        exit (0);
    }
    
    int kernel_ret;
    waitpid (k_info_pid, &kernel_ret, 0); // Wait until finished
    
    strcpy(out_k->portage_arch, architecture);
    
    return out_k;
}

response_t kernel_config (struct manager* m_man, int sc_no) {
    pid_t k_config_pid = fork();
    if (k_config_pid == 0) {
        chdir ("/usr/src/linux");
        char *args[3] = {
            "make",
            "menuconfig"
        };
        execv("/usr/bin/make", args);
        exit (-1); // Only return if fails
    }
    
    int config_ret;
    waitpid (k_config_pid, &config_ret, 0);
    return config_ret == 0 ? OK : INTERNAL_ERROR;
}
response_t kernel_build (struct manager* m_man, int sc_no) {
    ;
}