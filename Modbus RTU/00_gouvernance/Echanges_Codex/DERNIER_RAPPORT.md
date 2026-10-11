---
mission_id: TR2-20261011-DIAG-VDDIO2-RS485-007
status: BLOCKED
base_ref: main
base_sha: 4a0858725e6e562167ee585622f6e5e7687d2ab1
initial_head: 4a0858725e6e562167ee585622f6e5e7687d2ab1
result_sha: b35e236bfd88bbb4d7d2e5289add66c4505f0b0b
created_at_utc: 2026-10-11T12:00:36Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 007 — diagnostic VDDIO2 et sécurisation de l'essai RX

**BLOCKED provisoire : uniquement en attente de publication/relecture distante.**
Analyse et procédure de décision livrées suivant l'alternative explicitement
prévue au §4 du contrat. **VALIDÉ logiciel ciblé / COMPILÉ STM32 / NON QUALIFIÉ
électrique / NON QUALIFIÉ RX. Essai physique NON PRÊT.** Aucun patch d'activation
appliqué à une alimentation ou direction non vérifiée. La mission d'analyse
peut être clôturée sans prétendre avoir réparé/qualifié le banc.

## Références et livrables

HEAD initial propre main `4a0858725e6e562167ee585622f6e5e7687d2ab1` ; origin HTTPS
vérifiée ; fetch/pull fast-forward vers contrat `19aff5eebc4437cbabf2cb40bfdc7a3c56e9c464`.
Base du contrat ancêtre, sans merge/rebase/reset. AGENTS.md, protocole V2-B3,
journal intégral, mission/rapport006, préparation006, pinout, freezes P12F/P12G,
code STM32/HAL/CMSIS et procédure ST physique locale inspectés. Mission READY
ChatGPT inchangée ; les deux stashes005/002 sont préservés.

Commit livrables `b35e236bfd88bbb4d7d2e5289add66c4505f0b0b` — `Docs: diagnose VDDIO2 isolation and gate the next RS485 RX trial`.
Quatre fichiers : diagnostic `00_gouvernance/DIAGNOSTIC_VDDIO2_RS485_007.md`, journal
`00_gouvernance/ETAT_COURANT_TR2.md`, archives006 `MISSION.md` / `RAPPORT.md`.
Archives exactes depuis `4a085872...`, sans écrasement ni réécriture.
**Toutes les sources firmware, outils006 et interfaces sont inchangés.**
Le SHA ci-dessus désigne le livrable/documentation contenant les mêmes sources
firmware que celles réellement recompilées ; il n'est pas le SHA du firmware
programmé, toujours non identifié. Freezes, production désactivée et E4 inchangés.

## Conclusions fondées et limites

[ST AN5373 Rev7 §2.1.2](https://www.st.com/resource/en/application_note/an5373-getting-started-with-stm32u5-mcu-hardware-development-stmicroelectronics.pdf)
et HAL/CMSIS CubeU5v1.9.0 établissent : IO2SV bit29 de PWR_SVMCR est une commande
logicielle de levée d'isolation du domaine PG2..15, après présence effective
VDDIO2. Ce n'est pas une mesure ; le moniteur IO2VM vers0,9 V ne prouve pas un
rail3V3. La lecture utilisateur0 signifie bit non levé dans cette vue, sous
réserve de contexte/alias/protection/horloge valides. Aucun diagnostic de tension
absente déduit. Le futur boot ne conserve pas implicitement l'état observé.

[DS13737 Rev10 table28](https://www.st.com/resource/en/datasheet/stm32u575vg.pdf)
confirme PG7/PG8 AF8 LPUART1 TX/RX ; PG5 AF8=CTS, PG6 AF8=RTS/DE, pas une preuve
que PG4/PG5 candidats soient câblés à la direction. [UM2861 Rev10 tables9/19](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf)
confirme Morpho CN12-66/67/68/69 et sélection VDDIO2 via SB28/SB29 ; positions
réelles et rail distinct de tension ST-LINK restent à observer.

Lecture du code : PWR clock activée dans HAL_MspInit ; aucun EnableVddIO2 ni
écrivain SVMCR/IO2SV dans la plateforme ; PG7/8 déjà AF push-pull, UART8E1 et IRQ
raccordées, aucun DE//RE. **Initialisation locale incomplète pour usage PG7/8
au démarrage depuis reset avec IO2SV=0.** Ready006 prouve seulement start_receive
réussi, pas la disponibilité électrique des pins. L'appel HAL réellement
présent est identifié pour correction future **avant AF**, mais affecterait tout
le domaine et pourrait rendre PG7 active avec direction inconnue.

[Schéma MIKROE v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf)
identifie U1/pins de mesure ; [ADI RevD](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf)
confirme DE bas /RE bas pour RX, VIO logique distinct de VCC et seuils proportionnels
à VIO. Révision physique, routes et niveaux réels inconnus. Analyse extraite du
schéma ne prouve pas toutes les connexions graphiques VIO : cette limite est
explicitement conservée, aucune alimentation ni référence de masse inventée.

**Décision justifiée : pas de correctif actif, procédure de décision préparée.**
Un EnableVddIO2 inconditionnel ou un GPIO direction supposé ne peut pas être
déclaré sûr. Après un relevé minimal localisé VDDIO2, VIO/VCC, DE//RE, RXD et
références logiques, maintien sûr au boot établi, la tranche future pourra intégrer
l'appel HAL et sa politique de présence/échec si nécessaire. Aucun write GDB
pour fabriquer un essai réussi ; aucun firmware006 inchangé présenté comme corrigé.

## Nouvelles observations utilisateur et prochaine séance

Contrat007, provenance utilisateur : microSD **vide**, OpenOCD/GDB WSL attach,
halt et lecture réussis ; Flash157816 octets SHA256
`8869e369664da06a707d8d73633fd9fcf1e7940e985b963004c9da73d7b0f480`, différente du BIN006.
Le fichier `/tmp/tr2-rx006-loaded.bin` retrouvé a été lu/hashé **hors cible** par
Codex : même taille/empreinte ; aucune nouvelle opération MCU, aucune identité
fonctionnelle déduite de cet ancien dump. **Ne pas utiliser les symboles006 pour
interpréter la RAM actuelle.** Lecture `0x46020810 → 0` rapportée uniquement.

Reprise `monitor resume`, `detach`, `quit` invitée mais non confirmée : état du
CPU/serveur **NON VÉRIFIÉ**. La préparation commence par clarification/fin de
cette session dans une séance autorisée, sans lancer un second serveur, reset
ou flash à l'aveugle. Puis un relevé électrique unique ; décision/correction et
validation ; identification ou flash distinctement autorisé selon chaîne ST
qualifiée ; enfin AVANT→un seul envoi COM7→APRÈS selon006. Ne pas demander de
retrait microSD/recâblage inutile. La voie WSL observée pour lectures n'est pas
qualifiée pour flash/reset/reprise reproductible ; propriété ST-LINK Windows/WSL
à gérer explicitement après fermeture des sessions. Pas de transfert implicite.

L'essai reste NON PRÊT tant que firmware identifié/corrigé, IO2SV/contexte valide,
alimentation/direction et fonctionnement RX observables ne sont pas établis.
Le script HP reste NON VÉRIFIÉ en exécution complète (politique Windows006) ;
aucun contournement. Absence de réponse compatible avec RX seul, serveur/TX
et E4 non qualifiés. Journal actualisé avec ces faits et prochaines décisions.
À transmettre à ChatGPT : contrôler ce rapport puis cadrer les mesures humaines
et autoriser la correction/qualification matérielle sous contrat distinct.

## Validation réellement exécutée

```bash
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash ./tr2_validate.sh --cross-build-only
ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' \
  -R '^(codec|p12d_rtu_receiver)$' --output-on-failure
```

Cross-build007 retour **0**, compilation/link Cortex-M33 réussis ; **2/2 tests
existants codec/récepteur réussis** (binaires hôte005, sources inchangées).
Cross-build demandé explicitement par007 : vérifie reproductibilité de la
baseline ; il **ne teste ni ne corrige l'isolation matérielle**. Pas de tests
complets/.NET nécessaires pour documentation sans modification de cœur/interface.
text157692 / data124 / bss129072. Quatre warnings newlib usuels `_close`, `_lseek`,
`_read`, `_write` ; aucune erreur de build/test.

BIN157816 octets SHA256 `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d` ;
ELF `5ac5f0c66645692ff78579d792227ceb834f331baef9b8ed3a64d0ea55c293c3` :
**identiques006**, source C non modifiée. Log hors Git `/tmp/tr2-mission007-build.log`.
Contrôle offline de l'archive006 exact, diff/index/whitespace examinés et réussis,
comparaison octet pour octet fichiers indexés/locaux, recherche de secrets sans
occurrence, aucun build/artefact/secret hors périmètre indexé.

Limites documentaires rencontrées : RM0456 inaccessible via web puis timeout
sur téléchargement officiel ; AN5373/datasheet/HAL/CMSIS répondent aux questions
retenues. Aucun contenu RM0456 non lu ni règle d'un autre MCU utilisé. Extraction
textuelle du schéma ne suffit pas pour une preuve graphique d'alimentation ;
mesures/révision réelle restent prérequis. Aucun changement du poste ni installation.

## Publication et permissions

Commits et push normaux main autorisés et exercés ; **aucun flash, cible
OpenOCD/GDB, reset, écriture, émission physique ou manipulation**. Avant push :
main/destination, distant `19aff5e...` sans divergence, périmètre/secrets/index et
validations vérifiés. Stashes et mission ChatGPT inchangés.
Rapport BLOCKED provisoire : seule la preuve distante attendue pour clôturer
l'analyse préparatoire. Cycle V2-B3 prévu : publication, relecture intégrale des
livrables depuis origin/main, comparaison aux locaux, mise à jour DONE séparée
puis nouveau contrôle de publication et arbre propre.
