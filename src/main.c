#include <lt/io.h>
#include <lt/str.h>
#include <lt/sort.h>
#include <lt/math.h>
#include <lt/ctype.h>

#define ilesser(a, b) (a) < (b)
LT_DEFINE_QUICKSORT_FUNC(u64, sort_packed_uq, ilesser)

void day1(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	u64 list1[1024];
	u64 list2[1024];

	usz count = 0;
	for (char* it = input_data, *end = it + input_len;; ++count) {
		char* start = it;
		while (it < end && *it != '\n')
			++it;
		lstr_t line = lt_lsfrom_range(start, it++);

		if (!line.len)
			break;
		
		if LT_UNLIKELY (lt_lstou(lt_lstake(line, 5), list1 + count) || lt_lstou(lt_lsdrop(line, 8), list2 + count)) {
			lt_ferrf("failed to parse integer\n");
		}
	}

	sort_packed_uq(count, list1);
	sort_packed_uq(count, list2);

	usz occurences_size = sizeof(u64) * 100000;
	u64* occurences = lt_hmalloc(occurences_size);
	lt_mzero(occurences, occurences_size);

	u64 distance_total = 0;
	for (usz i = 0; i < count; ++i) {
		distance_total += lt_i64_abs(list1[i] - list2[i]);
		++occurences[list2[i]];
	}

	u64 similarity_total = 0;
	for (usz i = 0; i < count; ++i) {
		u64 left = list1[i];
		similarity_total += left * occurences[left];
	}

	lt_printf("total distance: %uq\n", distance_total);
	lt_printf("similarity score: %uq\n", similarity_total);
}

b8 day2_is_safe(u64* ents, usz count, usz mask) {
	usz unmasked0 = 0 + (mask == 0);
	usz unmasked1 = 1 + (mask == 1) + unmasked0;

	u64 prev = ents[unmasked0];
	u64 rising = prev < ents[unmasked1];
	for (usz j = unmasked1; j < count; ++j) {
		if (j == mask) {
			continue;
		}

		u64 current = ents[j];
		u64 delta = lt_i64_abs(current - prev);
		if (delta < 1 || delta > 3 || (prev < current) != rising) {
			return 0;
		}
		prev = current;
	}
	return 1;
}

b8 day2_is_safe_dampened(u64* ents, usz count) {
	if (day2_is_safe(ents, count, -1)) {
		return 1;
	}

	for (usz i = 0; i < count; ++i) {
		if (day2_is_safe(ents, count, i)) {
			return 1;
		}
	}
	return 0;
}

void day2(lstr_t input_path) {
	void* input_data = NULL;
	usz input_len = 0;
	if LT_UNLIKELY (lt_fmapallp(input_path, &input_data, &input_len)) {
		lt_ferrf("failed to memory map input file '%S'\n", input_path);
	}

	u64 entry_counts[1024];
	u64 reports[1024][8];

	usz count = 0;
	for (char* it = input_data, *end = it + input_len;; ++count) {
		char* line_start = it;
		while (it < end && *it != '\n') {
			while (it < end && *it == ' ')
				++it;

			char* num_start = it;
			while (it < end && lt_is_digit(*it))
				++it;

			if LT_UNLIKELY (lt_lstou(lt_lsfrom_range(num_start, it), reports[count] + entry_counts[count]++)) {
				lt_ferrf("failed to parse integer\n");
			}
		}
		if (line_start == it++)
			break;
	}
	
	u64 safe_total = 0;
	u64 safe_dampened = 0;
	for (usz i = 0; i < count; ++i) {
		safe_total += day2_is_safe(reports[i], entry_counts[i], -1);
		safe_dampened += day2_is_safe_dampened(reports[i], entry_counts[i]);
	}
	lt_printf("total safe: %uq\n", safe_total);
	lt_printf("total safe (dampened): %uq\n", safe_dampened);
}

int main(int argc, char** argv) {
	//day1(CLSTR("input1.txt"));
	day2(CLSTR("input2.txt"));
	return 0;
}

