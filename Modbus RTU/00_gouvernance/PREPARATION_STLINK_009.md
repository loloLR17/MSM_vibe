# Préparation ST-LINK / sauvegarde / flash008 — mission009

11 octobre2026. **Diagnostic hôte effectué ; aucune connexion MCU, sauvegarde, programmation ou émission exécutée.**

## Résultat hôte

usbipd5.3.0 : bus2-3, USB0483:374e, SN003500463235510B37333439, Attached à WSL. lsusb voit STLINK-V3 ; aucun OpenOCD/GDB actif détecté sous Linux ou Windows. CubeProgrammer2.23.0 `-l st-link-only` ne trouve aucune sonde. Détachement ciblé `usbipd.exe detach --busid 2-3` réussi sans élévation ; état ensuite Shared, sonde absente de lsusb, CubeProgrammer détecte SN attendu, V3J15M7, NUCLEO-U575ZI-Q. Cette option d'inventaire est explicitement sans connexion/intrusion cible dans l'aide CLI. Aucun unbind ni changement persistant de configuration ; sonde laissée disponible à Windows.

## Préparation ultérieure, non exécutée

Préconditions : mission distincte autorisant accès cible et flash/debug selon AGENTS.md, microSD absente confirmée avant boot, rail et J2/DE//RE selon [bring-up008](BRINGUP_RS485_RX_008.md), exclusivité sonde Windows (pas OpenOCD/GDB simultané). Relire qualification physique T1000 du10octobre et vérifier identité/SN/artefacts.

PowerShell : chemins artefacts Windows à définir via `wslpath -w` sur les fichiers réels, sans utiliser une ancienne copie. CLI vérifiée :

```powershell
$tr2Cli = 'C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe'
# Commandes préparées UNIQUEMENT : nouvelle permission cible requise.
# $tr2Backup et $tr2Bin : chemins Windows vérifiés avant exécution.
& $tr2Cli -c port=SWD sn=003500463235510B37333439 mode=HOTPLUG -u 0x08000000 0x200000 $tr2Backup
# Vérifier code retour, sauvegarde 2097152 octets, SHA256 ; conserver hors Git.
& $tr2Cli -c port=SWD sn=003500463235510B37333439 -w $tr2Bin 0x08000000 -v
```

La sauvegarde HOTPLUG2Mio est la méthode de la qualification locale ; elle implique accès SWD, donc interdite en009. Aucun `-e all`, Option Bytes, protection, upgrade sonde ou reset ajouté aux commandes préparées. Examiner les effets de connexion/programmation de la procédure qualifiée avant autorisation ; ces lignes ne qualifient pas un nouveau cycle. Après programmation, relecture Flash des157944 octets et comparaison byte-à-byte/hash BIN008 avant symboles ELF008. Ne pas utiliser l'ELF006 pour le firmware actuel non identifié.

BIN local `05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin`,157944 octets, empreinte recontrôlée009 : `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b`. ELF008 attendu `6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e` (référence rapport008, à recontrôler avant debug). Code source008 `0d4dde6cf31508ab54911f976f51f45c2f819152` inchangé par009.

Essai RX ultérieur : suivre [procédure008](BRINGUP_RS485_RX_008.md) et scripts006, test offline HP préalable, AVANT→reprise→un seul envoi COM7 1152008E1 `01 03 03 E8 00 01 04 7A`→attente1s→APRÈS. Attendus +8 octets/+1 ADU valide exacte/zéro erreur ; aucune réponse attendue. Permission émission/debug distincte nécessaire, aucune qualification RX en009.
