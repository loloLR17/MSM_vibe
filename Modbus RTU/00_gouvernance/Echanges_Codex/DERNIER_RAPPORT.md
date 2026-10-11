---
mission_id: TR2-20261011-STLINK-009
status: DONE
base_ref: main
base_sha: null
initial_head: 000b51545c5ebdd7625cab0234dfa4e569fd9b63
mission_sha: 7f46035283c33625c383cfedb04a7288555045a9
result_sha: null
created_at_utc: 2026-10-11T12:53:00.189302+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission009 — diagnostic ST-LINK autonome

**DONE : diagnostic hôte résolu, publication et relecture distante effectuées ; aucune qualification MCU/RX.**

HEAD initial main propre `000b51545c5ebdd7625cab0234dfa4e569fd9b63`, fetch/pull fast-forward vers `7f46035283c33625c383cfedb04a7288555045a9`. Contrat READY inchangé. Champs base_sha et created_at_utc absents du contrat : absence signalée, base_sha null sans invention ; références observées consignées, permissions/périmètre sans ambiguïté. result_sha null : aucun changement logiciel cible ni évaluation nouvelle de code firmware.

AGENTS.md, protocoleV2-B3, journal intégral, rapport008 et procédures T1000 relus. Qualification physique locale du10octobre et aide CLI actuelle inspectées. Archives008 exactes issues de `000b51545c5ebdd7625cab0234dfa4e569fd9b63`, sans réécrire les preuves historiques. Deux stashes005/002 conservés.

## Commandes et observations effectives

- `lsusb` : STLINK-V3 USB0483:374e dans WSL. `usbipd.exe list` : bus2-3 Attached. `usbipd.exe state` : SN003500463235510B37333439, client WSL présent, IsForced=false. Pas de sélection sur simple présence USB.
- Processus Linux via `ps` et Windows via `Get-CimInstance Win32_Process` : aucun OpenOCD/GDB/serveur ST actif détecté. Service usbipd actif ; ancien installateur CubeCLT présent, laissé intact. Requête PnP Windows sur VID0483/PID374e sans résultat pendant attachement WSL ; ne prouve pas un problème pilote.
- CubeProgrammer Windows CLI2.23.0 `--help` : `-l st-link-only` inventorie sans connecter/intruser cible. Exécution avant : **No ST-Link detected!**
- `usbipd.exe detach --help` confirme retour Windows après détachement non forcé. **`usbipd.exe detach --busid 2-3` exécuté, retour0**, opération hôte réversible dans le diagnostic demandé, sans intervention physique ni accès SWD. Aucun privilège administrateur requis, aucune politique/liaison persistante changée, aucun unbind ni arrêt forcé.
- Après : `usbipd.exe list` = Shared ; `lsusb` ne voit plus STLINK ; CubeProgrammer `-l st-link-only` détecte **SN003500463235510B37333439, FW V3J15M7, Board NUCLEO-U575ZI-Q**. Cause confirmée pour cette session : attribution USB WSL empêchant visibilité Windows. Sonde laissée disponible à Windows. GUI non inspectée ; résultat démontré par CLI du même environnement ST.
- `sha256sum` BIN008 local : **02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b**, concorde contrat/rapport008. Aucun nouveau build nécessaire.

## Livrables, validation et limites

`00_gouvernance/PREPARATION_STLINK_009.md` détaille méthode sauvegarde2Mio HOTPLUG, future programmation BIN008 à0x08000000 et vérification, exclusivité ST-LINK, empreintes et essai RX unique. Commandes préparées d'après aide CLI et procédure qualifiée, **non exécutées**. Sauvegarde actuelle NON EFFECTUÉE, Flash actuel NON IDENTIFIÉ ; nouvelle permission cible requise. Aucun usage de symboles ELF différent du firmware.

Journal actualisé : visibilité avant/après, propriétaire Windows, préparation et limitations. Archives008 mission/rapport exactes. Pas de source firmware ou test modifié ; inspection documentaire/diff et diagnostics hôte suffisants, aucune compilation/test firmware/.NET répété sans preuve utile. Les lectures d'aide/inventaire, détachement et contrôles ont réussi ; une sortie historique Flash UTF16 lue initialement comme texte affichait des caractères parasites, sans effet opérationnel ; conclusions fondées sur rapport physique lisible et aide actuelle.

Permissions exercées : diagnostic hôte non destructif, détachement USB réversible, documentation, commit et push normaux main. **Aucune connexion MCU, lecture Flash nouvelle, flash/debug/reset/écriture registre, émission COM7, manipulation physique, mass erase ou Option Bytes.** Aucune demande d'administration ou instruction physique nécessaire. RX et E4 restent NON QUALIFIÉS. Travail logiciel008 non refait.

À ChatGPT : contrôler le rapport distant, puis mission distincte accès cible/sauvegarde/flash/debug/essai RX avec conditions008. Publication du diagnostic ne qualifie ni firmware actuel ni réception.

## Publication

Push normal réussi sur main, HEAD=origin/main=`658dee47ec1dc558dd9a8b02da1c060819c65d7a` après fetch. Relecture intégrale des cinq livrables modifiés et de la mission inchangée depuis cette référence distante, comparaison byte-à-byte locale : concordance complète ; identifiant009 vérifié. Commit `Diagnose ST-LINK ownership and prepare future BIN008 qualification`. Mise à jour DONE publiée ensuite et contrôlée une dernière fois ; SHA final dans la réponse Codex.  diff/index et secrets contrôlés avant commit, destination main explicite, pas de force/merge/rebase/reset. Arbre initial propre, deux stashes préservés. SHA des commits fourni après publication ; pas de SHA auto-référent.
