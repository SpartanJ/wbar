#!/usr/bin/env bash

CFGDIR=/tmp
APPDIR=/usr/share/applications
ICONDIR=/usr/share/icons/hicolor
MAXICONS=10

apps=$(find $APPDIR -name '*.desktop' 2>/dev/null |
        xargs grep -l ^Name |
        xargs grep -l ^Icon |
        xargs grep -l ^Exec |
        xargs grep -l ^Type=Application |
        sort -R
     )

# print the dock background settings
cat > "$CFGDIR/wbar.cfg" <<-EOF
i: /usr/local/share/pixmaps/wbar/dock.png
c: wbar --bpress --above-desk --isize 64 --idist 5 --nanim 3 --pos bottom
t: /usr/share/fonts/TTF/UbuntuMono-B/12

EOF

for app in $apps; do
  _name=$(grep ^Name= $app | sed 's/^.*=//')
  _exec=$(grep ^Exec= $app | sed 's/^.*=//')
  _icon=$(grep ^Icon= $app | sed 's/^.*=//')
  icons=$(find $ICONDIR | grep -i "${_icon}")

  # get the biggest resolution icon
  for size in 128 64 48; do
    for icon in $icons; do
      if [[ "$icon" =~ $size ]] && [ -f "$icon" ]; then
        selected_icon="$icon"
        break 2
      fi
    done
  done

  if [ -s "$selected_icon" ]; then
    grep -q $(basename $selected_icon) "$CFGDIR/wbar.cfg" && continue
    cat >> "$CFGDIR/wbar.cfg" <<-EOF
i: $selected_icon
c: $_exec
t: $(echo $_name | sed 's/[^ a-zA-Z0-9.-]/./g')

EOF
  fi

  [ $((MAXICONS--)) -le 1 ] && break
done

# vim: set sw=2 sts=2 : #
