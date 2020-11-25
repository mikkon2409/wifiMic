#pragma once
#include "list_of_errors.h"

error_code send_all_files(volatile bool* need_to_stop);

void print_all_files();