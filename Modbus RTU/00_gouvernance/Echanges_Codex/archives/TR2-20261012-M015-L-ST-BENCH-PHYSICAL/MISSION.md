---
mission_id: TR2-20261012-M015-L-ST-BENCH-PHYSICAL
status: READY
base_ref: main
base_sha: fccb40cf799c753b9897e74ae6cc6e737a07383c
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-L — Qualification physique du banc de référence officiel ST

L'utilisateur annonce la seconde NUCLEO-U575ZI-Q raccordée au STEVAL-MKIGIBV2 + nappe + STEVAL-MKI208V1 et prête pour flash/tests. Lire AGENTS.md V2-B3, mission/rapport M015-K READY_FOR_PHYSICAL, PINOUT.md, PHYSICAL_PROTOCOL.md, AUDIT.md et HEAD main réel. Archiver K mission et rapport, préserver worktrees/stashes/preuves. Autorisations permanentes flash/debug applicables avec garde-fous.

## Objectif
Exécuter la qualification physique progressive du projet autonome experiments/iis3dwb_st_reference avec le **pilote et les séquences officiels ST** comme référence, sans changer les valeurs constructeur pour obtenir un succès et sans toucher au TR2 principal. Capturer les preuves et qualifier ou réfuter le fonctionnement de la chaîne IIS3DWB sur le banc isolé. Le firmware compilé M015-K est SPI3 PC9 CS / PC10 SCK / PC11 MISO / PC12 MOSI, mode0. Ne jamais supposer que le câblage de l'utilisateur correspond à cette table.

## Préflight obligatoire avant flash
1. Vérifier avec l'utilisateur si nécessaire la correspondance réelle des six nets et deux alimentations avec PINOUT.md, notamment **GND MKIGIBV2 identifié** et VDD/VDDIO=3,3V ; orientation de la nappe, absence d'autres modules. Si cette correspondance ne peut être établie à distance, état WAITING_FOR_WIRING_CONFIRMATION, aucun flash. Ne pas assimiler « branchée » à une vérification de pinout. L'utilisateur seul réalise les manipulations physiques.
2. Identifier par interrogation des outils matériels le SN STLINK et UID de la seconde NUCLEO ; refuser strictement les identités TR2 SN 003500463235510B37333439 et UID 004A003D3842501820333157. Ne jamais choisir automatiquement le premier STLINK. Contrôler tension, révision MCU et absence de risque de cible ambiguë. Si seule la carte TR2 est visible, arrêter.
3. Préserver la seconde carte : sauvegarde Flash 2MiB fraîche, double lecture et hashes, captures utiles. Vérifier artefacts binaires ST et identité avant flash. Aucun mass erase, HOTPLUG, Option Bytes/RDP, média ni modification permanente du TR2. Respecter la procédure CLI/GDB et restauration propre à cette seconde carte.

## Essais séquentiels
A. Flash NORMAL/verify/readback image WHO ST_STRICT, déclenchement unique, WHO_AM_I attendu 0x7B, aucun write capteur. Capturer HAL/CS/registre. Au premier écart STOP sans essai opportuniste.
B. Seulement si A conforme : polling officiel ST, 64 XYZ réels, états/reset/BDU, données brutes, erreurs et variations. STOP au premier écart.
C. Seulement si B conforme : FIFO ST_STRICT, niveaux, tags, horodatages et bruts. Conserver l'anomalie getter ST identifiée en K et ne pas la masquer. Le profil STATUS10 est une variante séparée, à n'utiliser que selon protocole documenté, en indiquant explicitement la différence avec le code officiel ; ne jamais présenter STATUS10 comme démo strictement identique. Respecter limites temporelles, une campagne bornée par étape, pas de répétition pour obtenir un résultat favorable.
D. Comparer résultats aux observations M015-I/J, sans attribuer prématurément un défaut au breakout, au SPI3 ou au logiciel. Aucun essai sur TR2 principal dans cette mission.

## Sortie
Publier traces brutes, snapshots, SHA, observations et limites dans preuves, DERNIER_RAPPORT.md, ETAT_COURANT et roadmap. DONE seulement pour étapes effectivement démontrées ; sinon WAITING_FOR_WIRING_CONFIRMATION / IN_PROGRESS / BLOCKED selon fait. Commit/push main, vérifier relecture distante. Respecter autonomie développement/flash/debug et ne demander à l'utilisateur que confirmation indispensable de câblage ou intervention physique.

## Clarification opérationnelle — doute sur le câblage
Si une incertitude de câblage, d'orientation, de masse, d'alimentation ou de correspondance des broches empêche le préflight, **ne pas abandonner la mission ni clôturer BLOCKED pour ce seul motif**. Suspendre uniquement toute mise sous tension supplémentaire, flash ou essai susceptible de présenter un risque ; demander directement à l'utilisateur une vérification physique ciblée et concrète (broche, photo, continuité hors tension ou tension mesurée selon le cas), avec la table attendue. Attendre sa réponse, puis reprendre au point de contrôle interrompu sans recommencer les travaux logiciels ni les essais déjà qualifiés. Documenter l'attente comme WAITING_FOR_WIRING_CONFIRMATION / IN_PROGRESS avec consigne de reprise. Ne jamais supposer qu'une connexion est correcte sans confirmation ; ne pas transférer à l'utilisateur les opérations Git ou logicielles réalisables par Codex.
