# R4-R — temps événement et essai B3 physique

Statut **BLOCKED**, R4 global/R6 ouverts. Rapport détaillé etpreuves :
[DERNIER_RAPPORT](../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md),
[preuvesR4R](../00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4R/README.md).
Code physique 0e808f3 ; candidate83d4fbd compilée/testée, jamais flashée.

## Méthode

ST [DS12569 Rev8§9.28](https://www.st.com/resource/en/datasheet/iis3dwb.pdf)
décrit compteur32bits et12,5 us nominal, ajustementFREQ_FINE. ST
[AN5444 Rev3§5.5/6.2/6.6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf)
décrit gel powerdown, slotsTAG_CNT et corrélation hybride avec timestamp décimé.
L'indication≈30h de cette AN contredit le calcul2^32×12,5 us≈14,91h ; ne pas
la coder comme horizon. Extensionmodulo32/demi-domaine et refusrecul utilisés,
aucun passage physique du rollover revendiqué. DriversST/parseurV1 inchangés.

Observation TR2 : TS précèdeXYZ du même TAG_CNT,24ticks/8 XYZ, BDR0xA/réservé0.
Provider buffer8XYZ, attendancre suivante, interpole dansla bracket réelle sans
ODR nominal/déqueue/UTC. Pas d’extrapolation initiale, pas de fenêtre terminale incomplète.
Le domaine timestamp capteur est relié àDWT via lectures compteur bracketées,
ratio observé ;100 us de garde expérimentale toutes les 100 ms, pas précision garantie.
Réinitialisation explicite pour un nouveau START, indices/run/config vérifiés parR3-B.

## Résultats et limites

- Diagnostic 01 interrompu parGDB trop tôt :52097 XYZ puisSPIerr1/STOP 235 us ; conservé.
- Diagnostic complet :2,000064 s/52894 XYZ/6612 TS,0 erreur, niveau 13/poll 401 us / gap 425 us,
 premier mot 10875 us ;197 brackets,12,54054 us/tick DWT, résidu22us/bracket33us.
 Fine1 lu seulement, pas étalonSI/UTC ou délai analogique qualifié. ArchiveR4-Q exacte.
- VraiR2 START 4/5/run 5 :118 XYZ/15 TS,112datés,6 nonbracketés ; **service2715>2500 us**,
 work922us/gap 2141us/niveau 64 ;3660RTU RAMcohérents/CRC contrôlés,0 B3 valide.
 STARTED 5 durable, pas COMPLETED/STOP tx6/10sreadonly/fenêtre 4096/HELD physique.
- Correction préparée : -O2 opt-in du chemin de banc etpublication avant requête,
 garde1000/2500/5000 inchangée. ARM/ASan+UBSan-O2/448 oracles PASS ; pas flashée.
 Snapshotguard refuse le nouvelétat14fb. Qualification de ce service puis décision
 humaine de traitementSTARTED requises avant nouveau START ; jamaisrejouer/effacer.

141 tests hôte62,28s/ARM etsanitizers ciblés ; V1 BIN85b1be60…c0f5 inchangé.
M016 Flashinitiale restaurée exacte2Mio,18 pages normales ; FRAMfinale14fb172f…2aa9
conservée sans rollback,113 octets modifiés uniquementLIVE/racine 39/CRCvalables.
Historique/tail exacts, capteur arrêté/WAIT/GDBdétaché. PasUART/RS485/SD/PC testés.
RMS/peak/Nquatrevaleurs/âgesHELD restent logiciel/àqualifierphysiquement ;
le préfixe112 ne qualifie ni dérivelongue, ni budgets continus, ni perteglobalezéro.
