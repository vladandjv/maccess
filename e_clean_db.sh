# Clenas the database and the shared memory if it exsists.

# Call it with "source e_clean_db.sh" in the directory where the 
# data files are.

rm -f *.dbc *.cdx
echo "The database files have been deleted!"
./bin/e_shmrm
rm -f *.log
