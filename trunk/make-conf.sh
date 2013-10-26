#!/usr/bin/env bash

CFGDIR=.
APPDIRS=(/usr/share/applications)
ICONDIRS=(/usr/share/icons/hicolor /usr/share/pixmaps)
MAXICONS=20

apps=$(find "${APPDIRS[@]}" -name '*.desktop' 2>/dev/null |
        xargs grep -l ^Name |
        xargs grep -l ^Icon |
        xargs grep -l ^Exec |
        xargs grep -l ^Type=Application |
        sort -R
     )

# print the dock background settings
cat > "$CFGDIR/wbar.cfg" <<-EOF
i: pixmaps/dock.png
c: wbar --bpress --above-desk --isize 48 --idist 5 --nanim 3 --pos bottom
t: /usr/share/fonts/TTF/UbuntuMono-B/12

EOF

for app in $apps; do
  _name=$(grep ^Name= $app | sed 's/^.*=\(.\{,10\}\).*$/\1/')
  _exec=$(grep ^Exec= $app | sed 's/^.*=//')
  _icon=$(grep ^Icon= $app | sed 's/^.*=//')
  icons=$(find "${ICONDIRS[@]}" | grep -i "${_icon}")

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
