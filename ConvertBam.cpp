#include <stdio.h>
#include <htslib/sam.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bam_file>\n", argv[0]);
        return 1;
    }

    const char *bam_filename = argv[1];

    // Open BAM file for reading
    samFile *bam_file = hts_open(bam_filename, "r");
    if (bam_file == NULL) {
        fprintf(stderr, "Error opening BAM file %s\n", bam_filename);
        return 1;
    }

    // Initialize bam header and alignment record
    bam_hdr_t *bam_header = sam_hdr_read(bam_file);
    bam1_t *alignment = bam_init1();

    // Read and process each alignment
    while (sam_read1(bam_file, bam_header, alignment) >= 0) {
        // Skip secondary alignments
        if (alignment->core.flag & BAM_FSECONDARY) continue;

        // Extract information
        int32_t tid = alignment->core.tid;
        int32_t pos = alignment->core.pos + 1; // 1-based position
        uint32_t mapq = alignment->core.qual;
	uint8_t *as_aux = bam_aux_get(alignment, "AS");
        int32_t score = 0;
        if (as_aux) {
            score = bam_aux2i(as_aux);
        } else {
            fprintf(stderr, "Warning: AS tag not found for this alignment.\n");
        }

        uint32_t *cigar = bam_get_cigar(alignment);
        int n_cigar = alignment->core.n_cigar;

        // Retrieve chromosome name
        const char *chrom_name = bam_header->target_name[tid];

        // Print extracted information
        printf("%s\t%d\t%d\t%d\t", chrom_name, pos, score, mapq);
        for (int i = 0; i < n_cigar; ++i) {
            int cigar_op = bam_cigar_op(cigar[i]);
            int cigar_len = bam_cigar_oplen(cigar[i]);
            printf("%d%c", cigar_len, bam_cigar_opchr(cigar_op));
        }
        printf("\n");
    }

    // Clean up
    bam_destroy1(alignment);
    bam_hdr_destroy(bam_header);
    sam_close(bam_file);

    return 0;
}
