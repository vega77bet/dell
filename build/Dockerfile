FROM debian:trixie-slim AS sdk
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends build-essential cmake ninja-build qt6-base-dev qt6-multimedia-dev qt6-svg-dev libqtermwidget-dev libx11-dev xvfb xauth openbox wmctrl xdotool python3 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
RUN apt-get update -qq && apt-get install -y --no-install-recommends libutf8proc-dev && rm -rf /var/lib/apt/lists/*

FROM debian:trixie-slim AS system
ENV DEBIAN_FRONTEND=noninteractive
RUN --mount=type=bind,source=out/stage,target=/stage \
    --mount=type=bind,source=os/overlay,target=/overlay \
 sed -i 's/Components: main/Components: main contrib non-free non-free-firmware/' /etc/apt/sources.list.d/debian.sources \
 && apt-get update && apt-get install -y --no-install-recommends \
 linux-image-amd64 live-boot live-config live-config-systemd systemd-sysv systemd-timesyncd dbus-user-session \
 xserver-xorg-core xserver-xorg-input-libinput xserver-xorg-video-all xinit x11-xserver-utils x11-utils \
 lightdm lightdm-gtk-greeter openbox wmctrl xdotool xterm zenity dunst trayer hicolor-icon-theme papirus-icon-theme fonts-noto-core fonts-dejavu-core \
 libqt6widgets6 libqt6svg6 qt6-svg-plugins libqt6multimedia6 libqt6multimediawidgets6 qt6-qpa-plugins qt6-translations-l10n libqtermwidget6-2 \
 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav gstreamer1.0-plugins-bad \
 pipewire-audio wireplumber alsa-utils pavucontrol network-manager network-manager-gnome \
 pkexec polkitd lxpolkit udisks2 gvfs-backends gvfs-fuse udiskie libpam-systemd \
 xfce4-power-manager lxrandr lxinput lxqt-admin brightnessctl rfkill xdg-utils xdg-user-dirs \
 calamares qml6-module-qtquick qml6-module-qtquick-controls qml6-module-qtquick-layouts \
 qml6-module-qtquick-window qml6-module-qtqml-workerscript \
 grub-pc-bin grub-efi-amd64-bin grub2-common efibootmgr dosfstools e2fsprogs parted rsync squashfs-tools \
 sudo locales tzdata util-linux-extra keyboard-configuration console-setup initramfs-tools zram-tools \
 firmware-iwlwifi firmware-realtek firmware-atheros firmware-misc-nonfree intel-microcode \
 firefox-esr firefox-esr-l10n-pt-br ca-certificates curl less nano procps iproute2 iputils-ping pciutils usbutils \
 && apt-get clean && rm -rf /var/lib/apt/lists/* \
 && cp -a /stage/. / && cp -a /overlay/. / \
 && find /stage /overlay -printf '/%P\0' | xargs -0 chown 0:0 \
 && find /stage /overlay -type d -printf '/%P\0' | xargs -0 chmod 0755 \
 && find /overlay -type f -printf '/%P\0' | xargs -0 chmod 0644 \
 && chmod 0755 /usr/bin/dell-session /usr/local/bin/dell-* /usr/local/sbin/dell-* /usr/local/bin/install-dell-os \
 && sed -i 's/^# pt_BR.UTF-8 UTF-8/pt_BR.UTF-8 UTF-8/' /etc/locale.gen && locale-gen \
 && echo 'LANG=pt_BR.UTF-8' > /etc/default/locale \
 && useradd -m -s /bin/bash -G audio,video,plugdev,netdev,sudo live \
 && echo 'live:live' | chpasswd && passwd -l root \
 && update-alternatives --install /usr/bin/x-terminal-emulator x-terminal-emulator /usr/local/bin/dell-terminal 90 \
 && chmod 0440 /etc/sudoers.d/dell-live \
 && systemctl enable lightdm NetworkManager systemd-timesyncd dell-ready.service \
 && systemctl mask apt-daily.service apt-daily-upgrade.service \
 && update-initramfs -u -k all \
 && : > /etc/machine-id && rm -f /var/lib/dbus/machine-id \
 && dpkg-query -W > /usr/share/dell-os/package-manifest.txt
CMD ["/sbin/init"]

FROM debian:trixie-slim AS iso-tools
RUN apt-get update && apt-get install -y --no-install-recommends xorriso grub-pc-bin grub-efi-amd64-bin grub-common mtools squashfs-tools qemu-system-x86 ovmf python3 && rm -rf /var/lib/apt/lists/*
WORKDIR /work
