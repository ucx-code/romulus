#!/bin/sh

TYPE=$1



if [ "$TYPE" == "ARTICLE_SINGLE" ]; then # Produce image for article but from a single run
   BASE_FOLDER=$2
   ID=$3

   python convert_sar_to_csv.py ${BASE_FOLDER}/sar_${ID}_*.bin -p cpu
   python convert_sar_to_csv.py ${BASE_FOLDER}/sar_${ID}_*.bin -r mem
   python convert_sar_to_csv.py ${BASE_FOLDER}/sar_${ID}_*.bin -b io
   python convert_sar_to_csv.py ${BASE_FOLDER}/sar_${ID}_*.bin -n_TCP net
   python sar_lines_for_article.py -c ${BASE_FOLDER}/sar_${ID}_*cpu.csv -m ${BASE_FOLDER}/sar_${ID}_*mem.csv -i ${BASE_FOLDER}/sar_${ID}_*io.csv -n ${BASE_FOLDER}/sar_${ID}_*net.csv -d in4
   rm ${BASE_FOLDER}/sar_${ID}_*.csv
elif [ "$TYPE" == "ARTICLE_DOUBLE" ]; then  # Produce image for article with comparison between two runs
   BASE_FOLDER_A=$2
   ID_A=$3
   BASE_FOLDER_B=$4
   ID_B=$5

   python convert_sar_to_csv.py ${BASE_FOLDER_A}/sar_${ID_A}_*.bin -p cpu
   python convert_sar_to_csv.py ${BASE_FOLDER_A}/sar_${ID_A}_*.bin -r mem
   python convert_sar_to_csv.py ${BASE_FOLDER_A}/sar_${ID_A}_*.bin -b io
   python convert_sar_to_csv.py ${BASE_FOLDER_A}/sar_${ID_A}_*.bin -n_TCP net
   python convert_sar_to_csv.py ${BASE_FOLDER_B}/sar_${ID_B}_*.bin -p cpu
   python convert_sar_to_csv.py ${BASE_FOLDER_B}/sar_${ID_B}_*.bin -r mem
   python convert_sar_to_csv.py ${BASE_FOLDER_B}/sar_${ID_B}_*.bin -b io
   python convert_sar_to_csv.py ${BASE_FOLDER_B}/sar_${ID_B}_*.bin -n_TCP net
   
   python sar_lines_for_article.py -c ${BASE_FOLDER_A}/sar_${ID_A}_*cpu.csv -c ${BASE_FOLDER_B}/sar_${ID_B}_*cpu.csv -m ${BASE_FOLDER_A}/sar_${ID_A}_*mem.csv -m ${BASE_FOLDER_B}/sar_${ID_B}_*mem.csv -i ${BASE_FOLDER_A}/sar_${ID_A}_*io.csv -i ${BASE_FOLDER_B}/sar_${ID_B}_*io.csv -n ${BASE_FOLDER_A}/sar_${ID_A}_*net.csv -n ${BASE_FOLDER_B}/sar_${ID_B}_*net.csv -d in4 -l full -l light -x 75

   python sar_stats.py ${BASE_FOLDER_A}/sar_${ID_A}_*cpu.csv ${BASE_FOLDER_A}/sar_${ID_A}_*mem.csv ${BASE_FOLDER_A}/sar_${ID_A}_*io.csv ${BASE_FOLDER_A}/sar_${ID_A}_*net.csv
   python sar_stats.py ${BASE_FOLDER_B}/sar_${ID_B}_*cpu.csv ${BASE_FOLDER_B}/sar_${ID_B}_*mem.csv ${BASE_FOLDER_B}/sar_${ID_B}_*io.csv ${BASE_FOLDER_B}/sar_${ID_B}_*net.csv
      
   rm ${BASE_FOLDER_A}/sar_${ID_A}_*.csv
   rm ${BASE_FOLDER_B}/sar_${ID_BID}_*.csv
else # Produce graph for internal consumption and analyzing the data set
   BASE_FOLDER=$2
   ID=$3

   python convert_sar_to_csv.py ${BASE_FOLDER}/sar_${ID}_*.bin
   python plot_sar_lines.py sar.csv timestamp ${BASE_FOLDER}/master.csv ${BASE_FOLDER}/sar_${ID}_*.bin ${BASE_FOLDER}/ping_${ID}_192.168.66.103_*.csv
   rm ${BASE_FOLDER}/sar_${ID}_*.csv
fi
#python plot_sar_lines.py sar.csv timestamp new_master.csv ${BASE_FOLDER}/sar_${ID}_*.bin ${BASE_FOLDER}/ping_${ID}_192.168.66.103_*.csv

