# If there is an error that says /mnt/tmpmp3 already exists
# run this script, and in git bash run 'git checkout student-distrib/mp3.img'
sudo rm -rf /mnt/tmpmp3
rm bootimg
rm mp3.img
# git checkout mp3.img
cp -rf V:\ece391\mp3\student-distrib\mp3.img mp3.img