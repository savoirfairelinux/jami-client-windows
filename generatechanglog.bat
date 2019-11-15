echo Start generating changelog
start pandoc -f markdown -t html5 -o changelog.html changelog.md
echo Changelog generated successfully