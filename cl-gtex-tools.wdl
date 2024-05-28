# Example workflow
# Declare WDL version 1.0 if working in Terra
version 1.0

workflow bamtotxt {

    input {
        File BAM
	File BAI
        String SAMPLE = basename(BAM, ".bam")
	Int cpu
        Int mem
	Int diskSizeGb
    }

    call bamToText {
        input:
        bam = BAM,
        bai = BAI,
        sample=SAMPLE,
	taskCpu = cpu,
	taskMem = mem,
	taskDiskSizeGb = diskSizeGb,
    }

    output {
        File out_txt = bamToText.out_txt 

    }
}

task  bamToText {
    input {
        File bam
        File bai
        Int taskCpu
        Int taskMem
	Int taskDiskSizeGb
	String sample
    }

    command <<<
        bamtotext.py -i ~{bam}  -o ~{sample}
    >>>

    output {
        File out_txt = "~{sample}.txt.gz"
    }

    runtime {
        docker: "mchaisso/cl-gtex-tools:v1"
        cpu: taskCpu
        memory: taskMem+"GB"
	disks: "local-disk " + taskDiskSizeGb + " LOCAL"
    }
}

