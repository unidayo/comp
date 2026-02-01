mkdir -p archive
dst="/home/wsl2ubuntu/comp/archive/contests_$(date +%Y%m%d_%H%M%S)"
mkdir "$dst"
mv /home/wsl2ubuntu/comp/contests/*/ "$dst/"
