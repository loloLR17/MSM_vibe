# M013-A — preflight TX et diagnostic FC03 isolé

11 octobre 2026. État historique M013-A, référence inspectée `6a35aefdef34b91ed82d217c37cabc4a04bf2e8f`, main : VIO Click et absence de commande concurrente RST non établis à cette date d'intervention. Tranche de diagnostic préparée et validée sur hôte ; aucun flash/debug/reset/émission en M013-A. Le dernier firmware identifié sur carte reste BIN008 (lecture011), sans nouvelle lecture013. Acquis RX011 conservé, pas de nouvel essai RX.

## Actualisation M013-C — 11 octobre2026 : GO statique, diagnostic flashé

**Le NO-GO RE/RST de M013-B est levé.** Confirmation utilisateur dans contrat C : broche2 RE/RST libre de toute connexion externe, y compris breakout/support, aucune liaison NUCLEO/autre sortie. Avec J2/J3 fermés, PG4 est le seul pilote du net DE–/RE–RE/RST ; schéma v102 relu. J1 position3,3V, VIO3,27V, DE=/RE=0V au repos sont des observations utilisateur acquises. Aucune répétition demandée. Les sections A/B ci-dessous décrivent leur état historique ; leurs interdictions temporaires ne constituent pas le verdict C.

**GO pour le flash standard effectué**, sous contrat `TR2-20261011-M013-C-FIRST-RTU-RESPONSE`. Candidat reconstruit depuis `fe1b9cf274d2fd51b82081e0a20c5e99dc55728c`, code inchangé depuis M013-A, diagnosticON/TXON/unité1 explicite. Build réussi, 3/3 tests ciblés réussis ; BIN48240 octets SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`, ELF `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`. Pas de boot/accès médias, aucune autorité production inventée.

Sonde Windows SN003500463235510B37333439, NUCLEO-U575ZI-Q/STM32U575 ID0x482 RevU,3,29V identifiés. Sauvegarde courante2MiB par deux lectures HOTPLUG identiques, SHA256 `67d505e6f03d52a868d2f87b8e0a6f3a9b209cd858f06dec8ee3087b8de31740`, image008 initiale reconnue. Flash0x08000000/verify/reset réussis, effacement normal secteurs0..5 ; readback2MiB SHA256 `be56e0699ea196329c7c3a1bca98743fbdeeb7ce8e7a7a5d370fb0c37c7a83de`, BIN exact, fin de page effacée et toute Flash à partir de0x0800C000 inchangée. Preuves hors Git `/home/lolo/dev/msm/reports/TR2-mission013-C-20261011T140835Z`.

**Boot observé via ELF exact** : ready1, poll_result0/error_count0 ; uwTick71285→75800 ; IO2SVactif, PG4latch/entrée bas, UART1152008E1/errorbits0. TimerIDLE, tx_busy/fault/bus_silent0, bufferTX et indicesfile0 lors des deux relevés. Aucun TX spontané observé par ces relevés et chemin logiciel sans émission non sollicitée ; absence de capture continue DE/bus, donc pas preuve instrumentée de tous les transitoires au boot. Sessions détachées, serveurs arrêtés normalement, cible en exécution.

**GO pour préparer l'unique probe HP**, sous réserve de fermer les autres applications série et de ne laisser actif que le maître HP prévu. COM7 inaccessible T1000 : aucun envoi automatique/COM3. Dans le dossier du script existant sur HP, une seule fois `./Send-Tr2RxProbe.ps1 -Transmit` (PowerShell accepte aussi `.\Send-Tr2RxProbe.ps1 -Transmit`), puis transmettre toute la sortie, sans retry. Requête `01 03 03 E8 00 01 04 7A`, réponse attendue `01 03 02 00 00 B8 44`. CRC indépendant calculé hors ligne : résidu0 pour chaque trame complète ; réception exacte permettra de contrôler CRC/unité/FC/valeurUNKNOWN0, en distinguant écho éventuel. Le message ancien du script « RX-only » ne décrit plus le firmware C.

**TX PHYSIQUE NON VÉRIFIÉ** tant que la réponse réelle n'est pas capturée. Si instrumentation déjà disponible sans modifier le montage : conserver DE/TXD/bus, DE haut jusqu'au dernier stopbit/TC puis bas et retourRX ; ne pas stopper le MCU durant l'échange. Un snapshot après échange contrôlera état final/erreurs/buffer, pas la chronologie physique des fronts. Aucun nouvel essai RX préalable, aucune mesure statique répétée. Jalon J1 global, reset pré-init, watchdogIRQ, production et E4 restent non qualifiés.

## Reprise M013-C après probe unique — état courant

L'utilisateur rapporte un seul probeHP effectué, CRC/self-checkOK, aucune réponse~1s. **Ne plus lancer l'instruction `-Transmit` ci-dessus** : essai consommé, aucune nouvelle émission autorisée dans cette reprise. Flashdiagnostic exacte reconfirmée ; UARTISR0x006000F8 avec RXNE/ORE, aucun buffer/fileRX/RTU/TX rempli, ready1 sanspollerror. LectureNVIC anormale ISER0/ISPRFFFFFFFF confirmée viaSTGDB et CubeProgrammer ; activation/visibilité à établir, pas une cause électrique prouvée. Diagnostic complet et prochaines actions passives dans `Echanges_Codex/DERNIER_RAPPORT.md`. BlocageRE/RST reste levé ; premierdialogue nonréussi, TXPHYSIQUE NONVÉRIFIÉ. Aucun nouveau contrôle statique demandé.

## Actualisation historique M013-B — 11 octobre2026

Contrat `TR2-20261011-M013-B-PHYSICAL-TX`, référence `70843b31a1fb1db631d1852a9384aa76136063cf`. Les lignes ci-dessous remplacent les lacunes statiques correspondantes de la matrice historique, sans réécrire les preuves M013-A archivées.

| Condition | Nouvelle preuve utilisateur, transmise dans le contrat M013-B | Conclusion permise |
|---|---|---|
| Révision / composant | MIKROE-3863 rev1.02, ADM2867E | Schéma v102 pertinent pour la topologie interne |
| VIO / J1 | J1 position3,3V ; VIO3,27V contre masse logique | Lacune VIO statique levée ; aucune nouvelle mesure demandée |
| Direction au repos | DE0V et /RE0V ; J2 et J3 fermés | Niveaux bas observés au repos ; pas preuve de transitions/turnaround |
| RST externe | Aucune liaison RST vers NUCLEO réalisée selon utilisateur ; point RST non identifié sur breakout | Absence de fil NUCLEO acquise ; liaison implicite par rangée/support encore à identifier |

**Topologie vérifiée sur le schéma :** PG4 → DE(U1-10) → J2 fermé → /RE(U1-9) → J3 fermé → contact RST mikroBUS. Ce contact est la **broche RE du Click, position mikroBUS2, voisine de DE/CS(position3)**, d'après la [table de pinout constructeur](https://www.mikroe.com/rs485-isolator-2-click). Il ne faut pas chercher un second reset du composant : RST est ici le nom du contact standard employé pour /RE. Aucun autre étage de sortie active n'est représenté sur ce net dans le Click v102 ; cela n'inspecte pas les rangées du montage externe. Une autre sortie maintenue basse pourrait entrer en conflit lorsque PG4 passe haut, même si les deux mesures au repos sont0V.

**NO-GO d'activation maintenu en attente de cette seule clarification :** « La broche RE du Click, position RST mikroBUS2 voisine de DE/CS, est-elle libre de toute liaison externe, y compris via une rangée de breadboard ou un support ? » Attendu : aucune autre liaison externe/sortie. Aucune coupure, mesure, dessoudage ou modification de J3 demandée. Si la route ne peut être identifiée, maintenir NO-GO. Question posée au cours de M013-B conformément au contrat§2 et AGENTS.md§9 ; les acquis VIO/DE//RE/J1/J2/J3/RX ne sont pas redemandés.

**BUILD/TEST M013-B réussis, FLASH/EXÉCUTION candidat/TX physique non effectués.** Reconstruction du code courant inchangé avec diagnosticON/TXON/unité1, CMake/Ninja et SDK Linux qualifié : BIN48240 octets SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`, ELF `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`, identiques M013-A ; trois tests ciblés diagnostic/transport gate0/gate1 réussis. Aucun nouveau code ni test modifié ; validation complète108/108 de M013-A reste une preuve antérieure. Absence de fonctions boot production/médias dans ELF recontrôlée. Preuves hors Git `/home/lolo/dev/msm/reports/TR2-mission013-B-20261011T135539Z`.

Si la topologie est confirmée sûre : appliquer la porte GO avant sauvegarde2MiB vérifiée, identificationSN/cible, flashstandard à0x08000000, verify/readback et debug avec ELF exact. Vérifier l'unicité du maître HP et absence de contention avant l'essai ; aucune émission spontanée. Un seul probe HP COM7 1152008E1 et réponse dorée indiqués à la fin de ce document, sans retry. Tant que NO-GO demeure, **ne pas lancer `-Transmit`**. CRC/réponse/timing réel/TC/retourRX devront être observés ; une future réponse seule ne prouvera pas la forme d'onde DE ni le comportement reset. Les portes et limites matérielles M013-A restent applicables.

## Matrice factuelle historique M013-A

VALIDÉ distingue ici une preuve logicielle d'une observation matérielle ; une déclaration utilisateur est identifiée comme telle. Les constats historiques restent datés, pas transformés en mesures013.

| Condition | État et source exacte | Lacune / risque | Moyen minimal / décision |
|---|---|---|---|
| Carte / composant | Utilisateur : MIKROE-3863 avec ADM2867E, rectificatif inventaire dans `ETAT_COURANT_TR2.md`, mission005 | Révision physique non identifiée ; anciens freezes ADM2587E historiques | Relever la révision lisible pendant la seule inspection ci-dessous, sans refaire l'inventaire |
| VDDIO2 MCU | Utilisateur008 : logique3,31V, SB28 fermé3,31V aux deux bouts, SB29 ouvert ; debug010/011 IO2SV=0x20000000 | Ni SB28 ni IO2SV ne mesurent VIO Click | Acquis conservé, aucune nouvelle mesure SB28/SB29 demandée |
| VIO Click / sélection | Schéma officiel v102 : U1-7 VIO = VCC sélectionnée par J1 ; constructeur nomme JP1 | Tension réelle inconnue ; sélection5V incompatible avec garantie d'un niveau haut MCU3V3 | Mesure VIO/GND1 au repos, attendu voisin3,31V ; NO-GO tant qu'absente |
| PG4 → DE | Fil PG4 confirmé utilisateur008 ; debug010/011 PG4 output/pulldown/latch et entrée digitale bas | Lecture digitale MCU ne mesure pas DE U1-10 | Mesurer au repos DE avec /RE dans le relevé unique ; ne pas forcer PG4 haut |
| J2 commun DE–/RE | Pont soudé déclaré utilisateur008 ; v102 relie DE10 à /RE9 par J2, table fabricant nomme JP2 | Déclaration de soudure conservée ; continuité électrique et adéquation à la révision non mesurées | Ne pas redemander « J2 est-il soudé ? ». Relevé des deux niveaux bas + inspection de route ; continuité reste à établir si incohérence |
| J3 / RST | Schéma v102 : J3 relie /RE à la broche RST mikroBUS ; table fabricant J3 peuplé par défaut | /RE sans fil externe ne démontre pas que RST est libre via support/socket ; deux sorties opposées peuvent se trouver reliées | Inspection de la connexion réelle RST et état J3, sans déplacer de pont ; NO-GO si commande concurrente ou route inconnue |
| PG5 / /RE externes | Utilisateur008 : aucun fil PG5 ni /RE ; code ne pilote pas PG5 | N'exclut pas la route indirecte J3–RST | Conserver ces acquis ; vérifier uniquement la route RST manquante |
| Half-duplex bus | Utilisateur006 : A↔Y, B↔Z en continuité hors tension ; A/B≈110Ω ; câblage blancA+, vertB−, noirGND, rouge5V isolé | Mesures historiques ne qualifient pas forme d'onde TX ni totalité bias/terminaison ; ADM2867E a TX/RX séparés | Pontage fonctionnel rapporté conservé ; contrôle dynamique au seul futur essai, sans recâblage ou nouvelle mesure A/B |
| Inversions / isolement | v102 : INVD/INVR tirés à GND1 par R15/R16 ; GND1 logique distinct de GND2 bus | État de carte réelle et reset non mesurés | Inspection selon révision ; ne pas relier les masses ni modifier INVD/INVR |
| RX fonctionnel | Rapport011 : +8 octets, +1 ADU valide exacte, CRC/UART0, envoi HP unique | Ne prouve ni niveau VIO, ni émission, ni sûreté reset | Acquis pour cet essai seulement ; aucun RX répété |
| DE / TC / retour RX | Transport012 et tests013 : DE haut seulement après requête/T3.5, bas sur TC, RX puis garde1750µs | Timing physique non observé ; watchdog50ms dépend des IRQ ; reset avant init sans garantie matérielle | Instrumenter DE/TXD/bus lors du prochain jalon ; aucune garantie permanente revendiquée |
| Image / symboles | Lecture011 BIN008 SHA25602ed2cf6…, ELF0086546629b… ; copies012 préservées et rehashées013 | Les nouveaux ELF013 ne correspondent pas à l'image008 sur carte | Aucun usage ELF013 sur la carte ; nouvelle sauvegarde/flash/readback exact avant futur diagnostic013 |
| Application / adresse | Hook faible courant NOT_AVAILABLE ; debug010 binding=3 ; aucune adresse/identité production acquise | TX ON seul ne donne aucun serveur opérationnel | Diagnostic isolé décrit ci-dessous ; production reste indisponible |
| Accès / sécurité | Chaînes ST Windows qualifiées010/011 ; microSD absente rapportée010 ; HP COM7 inaccessible depuis T1000 | Boot du harness peut toucher les médias ; COM3 est ST-LINK, pas FT232R | Diagnostic013 évite tout harness média ; aucune opération cible ici, aucun essai HP automatique |

## Sources constructeur effectivement relues

[Schéma officiel Click v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf), graphique intégral relu depuis copie008, empreinte conservée dans preuves013. L'ouverture web013 a échoué ; cette lecture locale est distinguée d'un nouveau téléchargement. [Table MIKROE](https://www.mikroe.com/rs485-isolator-2-click), consultée013 : JP2 contrôle commun, J3 contrôle indépendant, J7/J8 half-duplex. Ce nommage ne certifie pas la révision du montage.

[ADI ADM2867E RevD](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf), tables1/2/12 : U1-7 VIO, U1-9 /RE, U1-10 DE, GND1 côté logique ; VIL≤0,3VIO, VIH≥0,7VIO ; DE bas inhibe TX, /RE bas autorise RX. À VIO5V, VIH3,5V n'est pas garanti par MCU3V3. Délais DE enable/disable max40ns dans les conditions constructeur, pas mesures du banc. À VIO≈3,31V, bas≤0,993V et haut≥2,317V sont seuils logiques, pas critères suffisants de sûreté globale.

Documents de câblage relus : `BRINGUP_RS485_RX_008.md`, `DIAGNOSTIC_VDDIO2_RS485_007.md`, `FREEZE_FIRMWARE_P11A_STM32_BSP_PINOUT.md`, `INTEGRATION_RTU_TX_012.md`. Les anciennes références de transceiver/périphériques restent historiques ; aucun freeze réécrit. Constats011 et sauvegardes010 réutilisés avec leur provenance.

## Binding production et choix minimal

Sources inspectées : `platform/stm32/stm32_modbus_application.c`, `stm32_production_runtime.{c,h}`, `main.c`, `src/application/modbus_system_server.c`, RTU/PDU/read_adapter/projection, spécification normative `01_Specification_source/bloc1.md` et tests associés.

Le hook faible refuse avant toute préparation. Un binding production doit fournir transport qualifié, adresse1..247, dépendances durables, géométrie FRAM réelle et valide, CampaignDataStore réel et environnement configuration. Le propriétaire récupère une image transactionnelle valide ; EMPTY ne déclenche aucun format. Configuration/adresse/B0 ne sont pas déduites des fixtures du harness. L'identité est techniquement optionnelle dans la composition actuelle : sans source B0 la lecture correspondante retourne04, ce n'est pas un B0 provisionné. Readiness production est contrôlée avant init/start/poll ; aucun drapeau n'est forcé pour contourner ces conditions.

**Tranche minimale livrée : diagnostic de banc séparé, pas binding de production.** CMake `TR2_STM32_MODBUS_DIAGNOSTIC=OFF` et `TR2_STM32_RS485_TX_QUALIFIED=OFF` par défaut. L'activation du diagnostic requiert une adresse explicitement passée dans `TR2_STM32_DIAGNOSTIC_UNIT_ID` (défaut0 refusé, domaine1..247). Candidats compilés à unité1, marqueur de banc cohérent avec le probe006, jamais une adresse de production provisionnée.

Dans `main`, la branche diagnostic boucle immédiatement après HAL/horloges/alimentation/LED et avant SPI, capteur, FRAM, SD et binding de production. Elle ne formate, ne boote et ne modifie aucun stockage. Observables distincts `tr2_modbus_diagnostic_ready/last_poll_result/poll_error_count` ; ready du diagnostic n'est pas READY du capteur. Les fonctions média/boot production sont absentes des ELF diagnostic après garbage collection, vérifié par nm et désassemblage main.

`Stm32ModbusDiagnostic` utilise le vrai runtime RTU, CRC, filtres d'adresse/broadcast et PDU V1 existants. Seule lecture normale disponible : **FC03 registre1000 quantité1 → system_status=0 (UNKNOWN / initialisation incomplète)**, conforme B1§6.1 car aucun runtime de production n'est booté. Pas de statut NOMINAL, température0°C, charge0%, configuration/time/storage/READY ou identité inventés. Une lecture du reste de B1, même contenant1000, est indisponible (04) ; aucune image zéro complète n'est exposée.

Un callback optionnel `prepare_request` dans le runtime reçoit la vue ADU déjà acceptée (CRC/adresse/broadcast), avant PDU. Il permet cette disponibilité par requête sans dupliquer le serveur, ajouter de mapping ou supprimer le contrôle de readiness du serveur de production. Défaut NULL préserve le chemin existant ; ancien `prepare` inchangé. Le diagnostic remet son contexte PDU à zéro à chaque requête puis n'ouvre B1 que pour l'unique lecture connue. Toute autorité de commande, temps, configuration ou campagne reste NULL : FC16 RO→02, autorité absente→04, fonction non supportée→01 selon serveur V1 ; broadcast ne répond pas. Aucun retry après échec TX.

## Validations013 et limites

Validation complète justifiée par changement d'interface RTU partagée : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, retour0, **108/108 tests et cross-build défaut réussis**. Après extensions des tests uniquement : rebuild hôte puis CTest108/108 réussi. Deux builds ARM séparés diagnosticON / TXOFF et diagnosticON / TXON réussis ; configuration diagnosticON sans adresse explicitement fournie refusée comme attendu. Quatre warnings newlib historiques, aucune erreur masquée.

Tests nouveaux : vraie chaîne diagnostic/RTU/PDU avec sources manquantes, lecture UNKNOWN, non-exposition B1 entier/température/B0, exceptions d'écriture B1/B2/B5, FC06, CRC/adresse/broadcast, défautTX sans replay. Tests transport012 étendus : **diagnostic réel → vrai adaptateur STM32 sur HAL simulée**, requête et réponse dorées à unité1, aucune émission avant T3.5, gateOFF interdit TX, gateON/DE jusqu'au TC puis RX. Aucun test existant supprimé/affaibli. Aucun résultat simulé présenté comme observation électrique.

Artefacts hors Git dans `/home/lolo/dev/msm/reports/TR2-mission013-A-20261011T133639Z`, logs, disassemblages, symboles, `artifacts.json` et preuves de publication. Aucun binaire flashé013.

| Build | BIN octets / SHA256 | ELF SHA256 |
|---|---|---|
| Défaut diagnosticOFF/TXOFF | 157832 / `6e5518e4b7d8fbcf4b707662f64048e37506a6ec6d56002d55f8957a8a6fe299` | `00461f04f69b37b984d6fdde0ea37fbbdfadaa6f62ce80d97a38d7ba9a547f74` |
| DiagnosticON/TXOFF, unité1 | 46868 / `456d1c6146927c94a36438dc38b1b0231e5e83ca7057a3a77ca29efc7fcb379e` | `999ee3bf80ec06d0c522dac1b8be0aff2170ba3c31daa9b6b95413e80df340cd` |
| DiagnosticON/TXON, unité1, candidat non qualifié | 48240 / `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8` | `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a` |

## Intervention physique historique M013-A — mesures désormais rapportées dans M013-B

**Sans émission, reset, déplacement de pont ou recâblage, sur le montage au repos sous BIN008 :** relever au voltmètre continu **VIO (U1-7), DE (U1-10) et /RE (U1-9), référence GND1 logique du Click** ; préférer des pads accessibles dont la liaison est identifiée, ne pas sonder des pattes fines si risque de court-circuit. Attendus : VIO voisin3,31V, DE et /RE franchement voisins0V et sous0,3VIO. Dans la même inspection, relever la révision lisible et **si RST du Click rejoint un fil, socket ou une autre sortie, avec l'état visible de J3**. Ne pas mesurer de continuité sous tension. Si point inaccessible ou route impossible à identifier, rapporter ce manque sans improviser.

Motif : AGENTS.md§9 réserve ces observations physiques à l'utilisateur ; contrat M013-A§5 impose une seule intervention ciblée. Le schéma ne mesure pas VIO et l'absence de fil /RE ne prouve pas que RST est libre. **NO-GO maintenu** si tension incompatible, niveaux non sûrs ou commande RST concurrente/inconnue. Des valeurs statiques conformes permettront de réévaluer le préflight, sans qualifier à elles seules les transitions/reset/turnaround ; si révision/continuité demeure ambiguë, consigner précisément avant activation. Aucune mesure SB28/SB29, affirmation « J2 soudé » ou nouvelle réception RX demandée.

## Préparation historique M013-A — essai alors non autorisé

Après préflight sûr et sous-mission suivante : recontrôler contrat/SN/microSD et absence d'autre maître, sauvegarde2Mio vérifiée avant flash standard du candidat revu, verify puis lecture binaire exacte, ELF correspondant. Les permissions013 ne dispensent d'aucune de ces portes. Ne pas réutiliser une ancienne sauvegarde comme preuve de l'état courant. Instrumentation DE/TXD/bus pour départ et dernier stopbit, retour RX/garde ; pas de halt pendant échange et aucune liaison GND1/GND2 créée pour la mesure.

Un seul envoi HP COM7, 1152008E1 : `01 03 03 E8 00 01 04 7A`. Réponse diagnostic attendue : **`01 03 02 00 00 B8 44`**, CRC calculé hors ligne et réponse dorée testée. La valeur0 ne prouve pas l'état nominal ni la production. Pas d'envoi depuis COM3, pas de retry automatique ou nouvel essai RX préalable. Relever requête/réponse, CRC, compteurs diagnostic et erreurs, DE bas au repos/haut avant émission/haut jusqu'àTC/bas après puis RX. Le script006 sait émettre le probe mais ne constitue pas un validateur complet TX : aucun lancement automatique ici. Qualification timing/reset/endurance/E4 reste distincte.
