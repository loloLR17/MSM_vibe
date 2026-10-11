---
mission_id: TR2-20261011-RS485-FLASH-RX-010
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Mission 010 — Sauvegarde Flash, programmation BIN008 et qualification RX

## Décision et source de vérité

L'utilisateur a explicitement accordé le 11/10/2026 une autorisation permanente à Codex de flasher et debugger le TR2 quand cela est utile, sans allers-retours répétés. Voir AGENTS.md actualisé. Lire AGENTS.md, protocole V2-B3, mission et rapport009, PREPARATION_STLINK_009.md, BRINGUP_RS485_RX_008.md et les procédures matérielles T1000. Vérifier HEAD main réel, Git status et préserver les travaux préexistants.

## Situation qualifiée

Mission009 DONE : STLINK-V3 SN003500463235510B37333439 sur NUCLEO-U575ZI-Q rendu à Windows par usbipd detach bus2-3 ; STM32CubeProgrammer2.23.0 -l st-link-only le détecte. Aucun accès MCU ni flash effectué. BIN008 local Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin, taille157944 octets, SHA256 02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b. ELF008 SHA256 6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e. microSD absente et montage inchangé selon utilisateur ; DE breakout sur PG4, PG5 et /RE non câblés, pont J2 soudé.

## Exécution autonome

1. Vérifier état actuel USBIPD/sonde Windows, identité cible et correspondance image. Utiliser le STM32CubeProgrammer CLI Windows qualifié, depuis interop Windows/WSL si disponible ; ne pas réclamer de copier-coller à l'utilisateur pour une action réalisable par Codex.
2. Avant programmation, sauvegarder la Flash actuelle 2 Mio dans un emplacement unique hors Git selon PREPARATION_STLINK_009.md : commande qualifiée HOTPLUG -u 0x08000000 0x200000. Vérifier fichier, taille2097152, SHA256, et documenter restauration possible. Arrêter si sauvegarde impossible, protection active ou cible ambiguë.
3. Vérifier les effets de la programmation et la sûreté du montage. Flasher uniquement le BIN008 SHA vérifié à 0x08000000 avec -w et -v, sans mass erase, Option Bytes, RDP, déprotection ni manipulation physique. Vérifier résultat de programmation, puis relire les157944 octets et comparer byte-à-byte/empreinte au BIN.
4. Si préconditions de sûreté satisfaites, effectuer debug diagnostique en lecture avec ELF008 exact et snapshots de compteurs RX avant/après, sans faux appariement symboles/Flash. Vérifier PG4/DE bas, VDDIO2 et statut UART lorsque techniquement observable. Ne jamais activer TX TR2.
5. Si COM7 FT232R HP accessible et correctement identifié, réaliser un seul envoi Modbus RTU 115200 8E1 de 01 03 03 E8 00 01 04 7A, sans attendre de réponse. Résultat attendu +8 octets RX, +1 ADU valide, zéro erreur UART/CRC/overflow. Ne pas réessayer automatiquement en cas d'échec ; diagnostiquer et documenter.
6. Si un accès matériel ou des privilèges réellement indisponibles empêchent Codex d'agir, signaler une seule action utilisateur minimale ; ne pas simuler de résultat.

## Permissions et garde-fous

target_branch main ; allow_commit true ; allow_push true ; allow_flash true ; allow_debug true. Autorisation expresse permanente de l'utilisateur du11/10/2026. Autorisé : connexion SWD, sauvegarde, flash ciblé BIN008, vérification, debug en lecture, reprise contrôlée et un seul envoi COM7. Interdits : effacement massif, Option Bytes, RDP, déprotection, modifications de ponts/câblage, tests microSD destructifs, émission TX TR2, opérations Git destructives et force push. Toute intervention physique reste à l'utilisateur. Si un risque électrique sérieux est constaté, arrêter et remonter le point précis.

## Sortie

Publier DERNIER_RAPPORT.md mission010 avec états distincts BACKUP / FLASH / VERIFY / RX, preuves, fichiers et empreintes, commits, limites et prochaines actions. Commit/push normaux main et vérification distante obligatoires. ChatGPT contrôlera le rapport indépendamment.
