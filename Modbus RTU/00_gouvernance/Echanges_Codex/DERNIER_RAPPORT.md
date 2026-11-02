---
mission_id: TR2-20261101-R3-TR2-001-AUTONOME-REDUCTEUR
status: IN_PROGRESS
base_ref: main
base_sha: a8817bf293c6e60a93ad824d279ee870b6a0ecf7
initial_head: 79b952dbb949cb8eb2f208e3b2c5994f4ed1279f
result_sha: 171cefc3f8e6c02e37d58654e3ab05766f1a0241
created_at_utc: 2026-11-02T17:50:41.555777+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# CN0 chargé — comparaison deux heures sous CN15 seul, départ USER en attente

## CN0 — nouvel essai comparatif CN15 seul, chargé mais NON démarré

Demande utilisateur explicite après GI0 : nouvelle acquisition2h sans ST-LINK. Nom **CN00005.TR2** confirmé absent sur la SD512 FAT32/cluster32768, capacité/partition exactes,509038264320octets libres. Aucun fichier effacé/modifié, GI0 et anciens fichiers conservés.

Code source `6633b0154bf67b4c5437f1d259732fc1b1f21e4a`, cross-build ciblé ARM PASS, BIN53340octets/SHA `d31863809d62af9fc690e5ed1e8ef5fa2c3172c764774f0cb51f5e8e335cd396`. **Comparaison intégrale des BIN GI0/CN0 : seuls le préfixe GI0→CN0 et le SHA source embarqué diffèrent.** Durée7200000000µsMCU, allocation640MiB/±16g/488µgLSB/FIR107dec10/SPI3 8MHz/SD4MHz, tous contrôles qualité inchangés. Aucun ancien test rejoué ; compilation requise pour ce nouveau nom, aucune modification fonctionnelle.

Utilisateur confirme CN15+STLINK connectés/lecteur001vide/512auPC/JP6USB-C/JP2retiré. Préflash UID001/SN003500463235510B37333439/capteurwriteroff/nonincertain, programmationstandard à08000000 `-w -v -rst` réussie. Boot sanscarte stage99/error2/WHO7B attendu, SDwrites0/FIFOoff/writeroff/CShaute/GDBdétaché. Aucun START ni nouveau test effectué. Pas de masserase/OB/FRAM/COM4/002/protection modifiée.

**Étape physique en attente :** éjectionPC, coupureCN15+STLINK10s, réinsertion512, CN15secteurseul/STLINKdébranché, attendre WAITvert sansUSER/RESET. Puis USERphysique au signal, laisserCN15pendantcapture, noterheure PC. Ne pas connecter GDB pendantcapture. Àla fin conserverCN15 avantreconnexionSTLINK pourrecueillirRAM/digest/snapshot sansresetflash ; la reconnexion peut provoquer un reboot malgréJP2retiré (limite déjàobservée), snapshotNOLOAD utile mais non garanti après coupure. Aucune2hCN0 qualifiée ni départ réel revendiqués.

GI0 demeure : deuxheures atteintes/tousXYZ égauxdigestRAM mais échecqualitéfinalclips1 ; mouvement/choc volontaire possiblement rapporté paruser sanshorodatage, mitigation différée. Données/proofs précédentes préservées, suitephysiqueuserenattente.

[Preuves CN0](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/CN0-without-STLINK/ETAT.md).

## Résultats GI0 et checkpoints antérieurs

# GI0 — deux heures récupérées intégralement, un écrêtage : qualité refusée

## GI0 — récupération PC complète, qualité refusée

**Résultat final de cet essai : durée atteinte, intégrité du stockage vérifiée, qualité refusée.** TR2-001 / SN003500463235510B37333439 / UID004A003D-38425018-20333157, CN15 secteur + ST-LINK/GDB maintenus. USER physique ; aucun START agent, reset ou flash pendant la capture. Même code fonctionnel, source `171cefc3f8e6c02e37d58654e3ab05766f1a0241`, image GI0 53340 octets / SHA `e4c3f044d910d82722a468d2fd48726884c1dae10a27c387d1b9e8fc6e3bb0ce` (nouveau préfixe seulement).

- Durée observée par STM32 : **7 200 000 768 µs nominales**, pas une qualification SI. 190 351 880 natifs acceptés, **19 035 188 XYZ filtrés**, 11 sorties de préchauffage. Les deux natifs sans ancre future ne sont pas des sorties filtrées manquantes.
- `stage99/error26` : **une sortie filtrée écrêtée**, compteur rails0 ; 71 620 contrôles temporels/résidu maximum90µs≤100. FIFOerror0/stoperror0/SDerror0/CRCerror0/timeout0/queueoverflow0. STOP propre, vidage intégral puis refus qualité **avant** écriture du footer COMPLETE. Aucun affaiblissement du garde-fou.
- Carte transférée hors tension par utilisateur. **Copie intégrale 671 088 640 octets**, source SD ouverte en lecture seule ; SHA source/copie `46d2ad03588bb66bc84403374908a2cc92dd14c6671a3915f70dc532a3613f1f`. Les **244 041 records** passent CRC/session/ordre/temps/padding ; leurs **114 211 128 octets XYZ** ont pour SHA `fe2702106d36874a5e5d78a1db94550342badffb9c8b0b5eb4873ab064412611`, **identique au digest producteur indépendant sauvé en RAM avant coupure**. Après ces records, les 546 139 136 octets préalloués restants sont tous nuls. Lecteur strict inchangé : `INCOMPLETE`, pas de footer.
- Incident localisé dans le record115808 / sortie9033033, vers **56min56,47s** depuis le premier XYZ (interpolation MCU approximative). XYZ int16 = **[-21594, -32768, 32767]**, soit nominalement environ [-10,538 ; -15,991 ; +15,990]g à488µg/compte. Flags agrégés au bloc ; une sortie aux limites Y/Z retrouvée. Les blocs voisins et contexte CSV sont conservés. Écrêtage de sortie FIR ne prouve pas saturation physique du capteur ; les natifs de l'incident ne sont pas archivés, la cause mécanique/électrique/transport reste indéterminée.

**Interprétation :** ce rouge ne correspond pas à un arrêt précoce. Deux heures ont été produites et tous les XYZ sauvegardés, mais l'archive reste NON QUALIFIÉE pour la mesure prévue. Ne pas supprimer l'incident ni fabriquer un COMPLETE. Les anciennes pannes C29/B01/horloge restent distinctes ; ce succès de stockage en atelier avec ST-LINK ne qualifie ni l'environnement réducteur ni CN15 seul ni la powerbank. B02 60s COMPLETE reste acquis.

**État matériel final rapporté par utilisateur :** TR2-001 hors tension, microSD512 au PC ; GDB détaché avant retrait, FIFO arrêté, file vide, CS haute, writer non incertain. GI0 conservé en Flash, pas de restauration ni nouvel essai. Aucun accès002/FRAM/COM4 pendant GI0/récupération. Les 215XYZ RAM002 précédemment sauvés restent distincts ; OBE SD32 encore à récupérer, ne pas prétendre récupération002 complète. C29 zéroXYZ sauvegardé distinctement.

**Validation et publication :** aucune modification du firmware, aucun build ni test ancien rejoué ; lecteur strict sur fichier physique + contrôle indépendant de l'ensemble payload/CRC et SHA source/copie. Le lancement PowerShell `-File` a été refusé par ExecutionPolicy ; aucune politique/protection modifiée. La copie a utilisé l'invocation normale `-Command` déjà employée/qualifiée pour C29, sans `Bypass` ni élévation. Archives originales conservées localement sous `reports/TR2-001-autonome-reducteur-20261101/qualification-alimentation-20261102/GDB-USER-reproduction`, preuves compactes publiques.

Main a avancé de171cefc3 vers **ebae55e1** (projet shield uniquement). Autorisation explicite utilisateur d'intégration isolée : commits63d3846b et984d69c6 réappliqués dans un nouveau worktree baséebae55e1, dépôt/commits d'origine préservés. Branche locale `codex/tr2-001-GI0-publication`, destination autorisée `main`, push normal et relecture distante à contrôler. Rapport demeure IN_PROGRESS : essai GI0 terminé avec échec qualité, aucune campagne2h globalement qualifiée ni mission shield activée.

**Observation utilisateur après récupération :** il pense avoir volontairement bougé et frappé le capteur pendant cet essai. Cela rend une origine mécanique plausible, mais sans heure d’intervention la correspondance avec la sortie écrêtée à≈56min56s n’est pas établie. L’utilisateur reporte la mise en place de la solution d’écrêtage évoquée antérieurement ; aucune modification, reconstitution de cette solution de mémoire ou nouveau test dans cette tranche.

**Suite recommandée, non exécutée :** contrôle indépendant ChatGPT de ces résultats ; conserver l'incident et décider de la stratégie de robustesse/interconnexion. Un nouveau CN15 seul peut comparer l'influence du ST-LINK, mais n'est pas lancé et n'isolerait pas à lui seul l'origine de l'écrêtage. Aucune intervention physique supplémentaire demandée maintenant.

[Preuves PC et contexte de l’incident](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/GDB-USER-reproduction/PC-verification/RESULTATS.md).

## Checkpoints historiques conservés ci-dessous

# GI0 : durée deux heures atteinte, refus qualité final — récupération PC attendue

## GI0 — deux heures atteintes, refus qualité final (2 novembre 2026)

**FAIT GDB :** USER reconnu, préparation FAT32 terminée sans erreur, puis FIFO démarré. Le premier observateur à watchpoint s'était arrêté au WAIT par SIGTRAP ; ce n'était ni une erreur du firmware ni un départ. L'observateur suivant utilise seulement des breakpoints de fonctions : préparation avant FIFO et terminal après STOP, sans halt périodique pendant la capture. Le script publié est conservé tel qu'exécuté ; sa métadonnée descriptive héritée mentionne à tort les écritures d'état, absentes du script functions-only.

**ÉCHEC QUALITÉ, durée atteinte :** snapshot CRC conforme, source `171cefc3`, UID/SN001, durée MCU nominale 7 200 000 768 µs, 190 351 880 XYZ natifs acceptés / 19 035 188 XYZ filtrés. `stage99/error26`, une sortie filtrée écrêtée (`clips1`), rails0. La lecture du code montre que -26 intervient après vidage complet de la file et avant création du footer COMPLETE. Ce n'est pas une erreur de transport SD ni un arrêt anticipé démontré. Les vibrations/contact/capteur ne sont pas départagés par ce résultat.

**Autres observations :** 71 620 contrôles temporels, résidu maximum90µs sous100, FIFOerror0/stoperror0, stopped1, SDerror0/timeout0/CRCerror0, queueoverflow0. 244 041 records /124 948 992 octets de records écrits, payload114 211 128 octets. File vide, writer inactive/done/non incertain/sans cycle requis, CS haute, GDB détaché. Deux natifs sans ancre future restent non émis ; ce ne sont pas des sorties filtrées perdues. Digest producteur sauvegardé, snapshot et buffers conservés localement, copies compactes publiques.

**À DÉMONTRER :** copie PC entière de `GI00005.TR2`, CRC/séquence/session, comptage, comparaison SHA de l'ensemble XYZ avec le digest RAM, localisation du bloc écrêté. Aucun COMPLETE ni qualification des deux heures sans cette analyse ; aucune cause électrique exclusive. Ne pas relever les contrôles de qualité ni réécrire le fichier. L'utilisateur est invité à couper CN15/STLINK puis transférer la 512 Go au PC ; aucun nouvel essai lancé. Module002/FRAM/COM4 intacts pour cette observation.

### Checkpoint précédent conservé

# C29 récupéré sans données ; reproduction GDB001 préparée et chargée

**IN_PROGRESS : récupération001 achevée, 2h NONqualifiées, nouveau diagnostic demandé.**
Correctionuser : le coffretrouge était001/512Go. Les lecturesprécédentes avaient
explicitement choisiSN002 et décrivent son arrêtindépendant :47min35,97sMCU/error3,
résidu131µs>100/SD-FIFOerror0/4clips/7605527sorties, RAMet215XYZfin sauvés ; fichier
002 encore àcopier pourcomparerauproducteur. Aucuneimputation de cesrésultatsà001.

001 avait déjàété coupé pourdéplacementavantlecture. NouveaubootWHO0/run0/snapshot
invalide ne décritpas la campagne ; diagnosticRAMhistorique perdu. Aucunresetflash
pendantrecovery. 512Go copiéeenlectureseule, C290005.TR2 conservéentier671088640B,
SHA source=copie : `f7ca815f74b78c9e6a90f29c4ce26d5bc8f4c99e2b810fba10b1df82f1fc1ade`.
**En-têteCRCvalide, puis671088128octets tousnuls :0recordXYZ, aucunfooter.**
LecteurstrictINCOMPLETE etscanintégral concordants. Aucune mesure récupérableC29,
ni durée effective démontrée. Causeantérieure inconnue ; WHO0postcoupure n'est pas
une preuve de l'erreurinitiale. H2A/T10 préalloués640MiB/headersetqueues512Bnuls,
laissésintacts sansrevendiquerlescanscomplets. Taillepréallouée≠duréeenregistrée.
Représentationpubliqueexacte de C29 : en-tête512B +zéros, hashreconstruit=original,
bruteoriginale complèteconservée. Pas de suppression/formatage/réparation.

**Suite demandée et autorisée : GDBpersisitant sur001/CN15/USER.** GI0 est chargé,
programming-vvérifié, nouveau `GI00005.TR2` librePC ; source171cefc3/BIN53340B/SHA
`e4c3f044d910d82722a468d2fd48726884c1dae10a27c387d1b9e8fc6e3bb0ce`.
Même profil2h/640MiB/FS16/SPI3 8MHz/SD4MHz, aucunemodificationfonctionnelle.
Bootlecteurvide99/error2/WHO7B attendu, FIFOwriteroff/SDwrites0/CShaute/GDBdétaché.
Script prêt : observeUSER/entréepréparation/entréeFIFO etterminale ; pauses avantFIFO
ouaprèsSTOP, pasdepollingGDBhaltpendantcapture. AucunSTARTagent. PasencorearméavecSD.
Userinvitéàinsérer512horstension puisCN15+STLINKmaintenus sansUSER ; retourattendu.

[Résultats, distinctiondescibles et preuves](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/rearm-2H-C29/recovery-PC/RESULTATS_ET_ATTRIBUTION.md).
Journal actualisé ; module002/FRAM/COM4/anciensfichiers inchangés pourpréparationGI0.
B02minute CN15+SD COMPLETE etD07D08 restentacquis, aucune2hmesure valide revendiquée.

---

## Historique avant récupérationC29 et préparationGI0


# Mise à jour opérateur — fixation TR2-002 perdue pendant la campagne

**TR2-002 : campagne déclarée non exploitable pour la mesure vibratoire prévue.**
L'utilisateur rapporte que le capteur s'est déplacé et désolidarisé de son support
pendant l'acquisition OBE0005.TR2. Heure du déplacement inconnue ; l'heure de réception
du message dans OPERATOR_MOUNT_FAILURE.json ne constitue pas l'heure de l'événement.
Aucune qualification d'un préfixe antérieur n'est déduite. L'intégrité numérique et
la fin de l'enregistrement ne sont pas déterminées par ce constat mécanique.
Le lancement ACTIVE/SDACK de12:30UTC reste une preuve de départ, pas de tenue de lafixation.

AucunSTOP, nouveauSTART, reset, flash ou debugger sur les deux cibles. Aucune donnée
supprimée. Le repère « passage sur mode constante »12:33UTC reste un témoignage séparé,
sans lien temporel prouvé avec la perte de fixation. Pas de nouvelle analyse engagée.

**TR2-001 reste non observé :** départ12:12UTC déclaréuser, LEDéteintes ; les succès
D07/D08/B02 ne prouvent pas le départ effectif C29. Conserver CN15 au moins jusqu'à
14:22UTC, sans bouton ni reconnexionSTLINK ; organiser ensuite récupération sans
écrasement pour constater ce qui a réellement été enregistré. Aucune garantie dedonnées.

[Constat opérateur](preuves/TR2-20261101-001-AUTONOME/TR2-002-2H-OBE-20261102/OPERATOR_MOUNT_FAILURE.json).
Journal actualisé ; enregistrements et état logiciel laissés intacts.

---

## Historique avant le constat de fixation002


# TR2-002 — 2 h lancées, ACTIVE et première écriture SD confirmés

**IN_PROGRESS : lancement confirmé, fin nonobservée.** Extensionutilisateur explicite
à002 (« arme[…]2h[…]STLINK », puis « lanceaussitot[…]prêt »), aucunaccès001.
CibleSN0032001F3235510C37333439/UID001B0046/38425018/20333157. AncienneFlash2MiB
sauvegardéeSHA113f54b7…386e5, FIFO/writerinactifs avantprogrammation. BuildARMcibléPASS,
flashstandard vérifié ; source **dd2f27ca3d886742b0ab37ab378cd524012a0370**,
profilhistorique002 **±8g/244µgLSB/SPI3 8MHz/SD4MHz/FIR107Q15dec10**, garde2hnominales
7200000000µsMCU. Nouveau `OBE0005.TR2`/préallocation640MiB, gardesinchangées.

Init/calibration etCID32Go/capacité62333952/FAT32conformes, noms0003/0005absents et
906719clusterslibres pour20484requis. Gate2s requis unefois puisrelectureSDcontre
producteurRAM `durable_verify_last=0`. Délai dûauscriptGDB appelantunsymbole nonlié,
corrigé via procéduresmetadatahistoriques sansréinitialisationrépétée ; pas un blocage
matériel. Aucun testancien relancé ni contrôleassoupli.

**ACTIVErun5 et premiergroupeSDacquitté confirmés à12:30:12.839818UTC**, session
8444550119014285/accepted12808/flags1281. OrdreSTARTunique, GDBdétaché ensuite.
TémoinWindowsnatifindépendant PID15364/COM5 parent002, PCsecteur/veilleAC0 constatés.
IntervalleQPCintérieur demandé7100s, pas unequalification2hSI ; capturegarde2hMCU.
Journaux dans `C:\Users\lolo\AppData\Local\Temp\TR2-R3-N8G-20261101-OBE-2h`.
ConserverSTLINK002/T1000allumé sursecteur ; aucuneinterventionGDBpendantcapture.
Aprèsfin : vérifierterminal etSDquiet puis sauverdigestRAM avantcoupure/retraitSD.

**Repèreuser : « passage sur mode constante » à12:33UTC**, précisionminute,
conservételquel/scopephysique nonprécisé. Aucunchangementlogiciel pourcerepère.
TR2-001 reste indépendant : départ12:12UTC déclaré, LEDéteintes/étatréelnonvérifié ;
aucunelecture/reset/flash/commande sur001 pour ce lancement002. Ne pasfusionnerlespreuves.

[Checkpoint et reprise](preuves/TR2-20261101-001-AUTONOME/TR2-002-2H-OBE-20261102/DEPART_2H_ET_REPRISE.md).
HEAD initial `dd2f27ca3d886742b0ab37ab378cd524012a0370`, journal actualisé.
Publication `70a068a83f972005202b2362dd0c74f708058da4` relue :45fichiers identiques/839empreintes conformes,
worktreepropre ; voir publication.json et LAUNCH_CHECKPOINT.json. ARMED.json décrit
l’étapehistorique avantSTART, ne pasrejouer le lancement ou le gate lorsde la reprise.
Aucunformat/OB/FRAM/COM4/Windowssecurity/anciennecapture touché ; 2hNONterminées.

---

## Historique avant le départ OBE sur TR2-002


# Réarmement 2 h — C290005.TR2 chargé, départ manuel attendu

**IN_PROGRESS.** Sur demande expliciteutilisateur, réarmement seul : aucun essai ni
analyse supplémentaire, contrôledeprogrammation obligatoire conservé.
Cible001 SN/UIDvérifiés, FIFO/writerinactifs et nonincertains avantflash.
Durée7200000000µsMCU, nouveaupréfixealéatoireC29/fichier **C290005.TR2**,
FS16/SPI3 8MHz/SD4MHz, gardes etCREATE_NEWinchangés. Pasdegarantiecollisionparaléa
seul : l'absence du nomn'a pas étéinterrogée surSD, antiecrasementtoujoursactif.

Source **365b0ccfe3d3c80e0d0b894e7e99cf3e0f7b4a69**, BIN53340B,
SHA `3a1ec9ceb5f851250965c291f9101d6653dd4f3e9c2a0e4a7cd8767b02230fde`.
Unflashstandard avec-v/-rst etcontrôleprogrammateur réussi. AucunSTARTagent,
pasdelectured'étatpostflash ni calibrationcommandée/testd'acquisition.
Les BIN/ELF/MAP sontpréservéslocalement. Aucun002/FRAM/COM4/formatage/OB/suppression.

DépartparUSER2s aprèsinstallation/carteenplace/WAITvertclignotant ; secteur5Vstable
CN15 confirméprécédemment. Toutgestecarte/jumperhorstension. Ne pasutiliserRESETpendant
capture ; maintenirCN15aprèsfin ouerreur. L'étatWAIT/ledépart etles2h ne sontpasencore
observés. Qualification10minreportée paruser ; B02minuteCN15+SD COMPLETEpréservée.

[Preuves du réarmement](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/rearm-2H-C29/REARMEMENT.md).
Journal actualisé, aucun travailpréexistant écrasé ; mission et autorisations inchangées.

---

## Historique avant le réarmement C29


# T10 — refus confirmé de nom déjà présent, récupération du fichier requise

**IN_PROGRESS : aucune conclusion sur les 10 min avant lecture du fichier.**
User rapporte RESETaccidentel avantUSER, puis rouge pendantl'attente sansnouvelappuiUSER.
Au premier relevéSN001/UIDexact/BIN T10 entierinchangé, démarragelectureFATencours,
WHO7B/FS04/cal41µs, run0 et SDwrites0. **Aucun snapshotterminalvalide** : les valeurs
retained.payload imprimées sont ininterprétables et ne sont pas des résultats.
Le refus rouge antérieur n'est pas directement conservé ; rebootdereconnexion cohérent
avec historique, causenonexclusive/CSRcumulé. AucunresetflashSTARTagent.

Watchpointmatériel surchangementd'état pendantprécontrôle : **stage99/error1,
durable_status.error8**, espace libre15596052clusters pour20480requis, zéroerreurSD/FIFO/IO,
aucuneécriture. Ce8 est uncodeinterne de contrôledu nom, pas unFR_EXISTbrut.
Vérificationf_stat cibléelectureseule avecscratchRAMrestauré : **FR_OK, `T100005.TR2`
présent,671088640B (640MiB)**. Le refusactuel protège donc un fichier existant.

La taille est une préallocation, pas une preuve de durée ou de données. Une tentative
interrompue pourrait avoir créé le fichier avantceboot ; son origineexacte etsoncontenu
restentàdéterminer. Aucunessai10min validé, aucunéchecpendantacquisition démontré.
B02minuteCN15+SD COMPLETE etD07/D08 restentacquis, tousfichierspréservés.

FIFO/writerarrêtés, uncertainfalse/needs_cyclefalse/SDwrites0/CShaute/GDBdétaché.
UserinvitéàcouperCN15+STLINK ettransférer512GoPC, enattentederetour. Copieintégrale640MiB
etlecture stricte àeffectuer, sanssuppression/réparation/formatage/écrasement ni002/FRAM/COM4.
Aucun nouveautest ou nom contournantlaprotection avantrécupération.

[Preuves du refus et limites](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/storage-T10-CN15/red-before-USER/REFUS_CONFIRME_ET_RECUPERATION.md).
HEAD initial `c322a2fa55fd32e0970ab6a07daf17de4018817e`, sourceT10 d7bd4df0 inchangée ;
aucunbuild/testancienrépété. Journal actualisé avec refus de nom et récupérationprioritaire.

---

## Historique jusqu'au checkpointc322a2fa


# T10 chargé et relu — attente de carte et WAIT avant les 10 min

**IN_PROGRESS. B02 minute autonome CN15+SD reste qualifiée, T10 non démarré.**
UserconfirmeCN15+STLINKconnectés après instruction de garder512GoauPC/lecteur001vide.
UID001 et état sûr vérifiés, puis unflashstandard et readbackentier53340B exact.
Source **d7bd4df001576d5fef9b0af034b1cb96c505859a**, SHA-256
`8c4c12de78d54177bb99c564ce9acb8f803ff1576f0a50de55d8e598143b0948`.
BuildARM déjàréussi réutilisé ; pas de recompilation ni de test ancien répété.

Profil600000000µs/USERunique/nouveau `T100005.TR2`, FS16/488µgLSB,
SPI3 8MHz/SD4MHz, mêmes gardes, préallocation640MiB existante compatibleespace.
Bootsanscarte attendu99/error2/sd_error2/WHO7B ; FIFO/writeroff/nonincertains,
CShaute/SDwrites0/GDBdétaché. Aucune acquisition10min, aucun002/FRAM/COM4/formatage.

Transition physique encoreàfaire : éjecter512GoauPC, couperCN15+STLINK001/10s,
insérer512Go, garderJP6USB/JP2retiré, CN15secteur5Vseul/STLINKoff, attendreWAITsansUSER.
Aprèsconfirmationseulement, un départ10min puis diagnostics/relecturePCcomplète.
Aucun8hSTART. La validation60sB02 n'est pas répétée.

[Preuves flashT10](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/storage-T10-CN15/T10_FLASH_ET_TRANSITION.md).
HEAD initial `6dc17d559fd7710bd0ea207503c53f164d698d2c`, journal actualisé avec imageactive,
bootSDabsenteattendu et prochaineactionuser. B02/D07/D08 et brutes conservés.

---

## Historique jusqu'au checkpoint6dc17d55


# B02 — minute autonome CN15 + SD, fichier COMPLETE vérifié intégralement

**IN_PROGRESS global ; PASS physique de la minute avec enregistrement et lecturePC.**
Source réellement évaluée `3f803d77393c6853ed76d796367df443a0eb9eb9`, B02/FS16/SPI3 8MHz/
SD4MHz, même BIN53312B entièrement relu identique. UserRESET accidentel avantSTART,
retourWAIT puisUSERlong ; vertfixe, pas de reset pendantcapture rapporté.

SnapshotCRC50fbba78 conforme : **60,000254sMCU,158520XYZproduits, error0**,
597checks/résidumax28µs≤100,0FIFO/SD/STOP/rails/clips/flags/queueoverflow/budgetmask,
writerterminé sansincertitude/filevide. Digest producteurRAM sauvegardé avantcoupure.
Reconnexion suivie de reboot/scanFATlectureseule ; snapshotrestévalide. Attente de fin
puis refus attenduFR_EXIST(8) du nomdéjàprésent. FIFO/writeroff/SDwrites0/CShaute,
GDBdétaché avanttransfert physique. Pas de nouvelordreSDagent ni nouveauSTART.

**B020005.TR2 relu COMPLETE surPC :2033records/158520XYZ, tous CRC, séquences,
session, durée, UID/source et profilFS16/488µgLSB conformes.** Digest globalXYZ égal
au producteurRAM : `d8afd2a04abc4a987577d8806f90e45896ab8c8bf1b49d44bda051737e2b0d69`.
Copieintégrale2097152B/source-destinationSHAidentiques :
`d49dbd86954125c37563f0418e5af4744ec2851b5a4acf8c1af7766f35b680d4`.
Parcoursindépendant desoctets reproduit les2033CRC/séquences/comptes/digest.
**Neufanciennescaptures A1A..H/B01 inchangées.** Aucunformat/réparation/suppression,
aucune002/FRAM/COM4. Les données brutes restent localementT1000, preuvescompactes
GitHub ; recontrôleindépendant exhaustif nécessite la copiebrute identifiée.

État : 001hors tension,512GoauPC, B02enFlash. Capture60s seule qualifiée, pasendurance8h,
powerbank ou implantation enmachine. Le format conserveDC ; aucune modificationnormative.

**Prochaine tranche10min PRÉPARÉE, NONFLASHÉE/NONDÉMARRÉE :** buildARMcibléPASS,
source `d7bd4df001576d5fef9b0af034b1cb96c505859a`, mêmes sourcesfonctionnelles normales001,
optionsdiagnostiquesOFF, durée600000000µs/préfixeT10, nouveau `T100005.TR2` absent,
préallocation640MiB existante compatibleespace. Aucunancien testhôte relancé.
Carte restantPC, il faut reconnecter001vide parCN15puisSTLINK pour charger l'image.

[Preuves complètes, limites et artefactT10](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/storage-B02-CN15/B02_COMPLETE_PC_ET_SUITE_T10.md).
HEAD initial de tranche `d7bd4df001576d5fef9b0af034b1cb96c505859a`, journal actualisé.
Checkpoint `c4638d7103b6317822d61a0bb371792b2aaa1a7c` publié et relu : 34 fichiers identiques,
754 empreintes conformes, worktree propre. Voir `storage-B02-CN15/B02-PC-remote-verification.json`.

---

## Historique jusqu'au checkpointd7bd4df0


# Reprise stockage — B02 chargé, attente de carte et de WAIT sous CN15

**IN_PROGRESS. D08 sans SD est acquis, stockage pas encore qualifié.**
Le checkpoint D08 `c0a847438496f77659309bbab6d1f3b84d1d7784` est publié et relu : 18 fichiers
identiques/709empreintes conformes ; preuve `CN15-D08/remote-verification.json`.
Résultat physique D08/sourceacb14279 conservé ci-dessous, aucun nouvel essai sansSD.

Image stockage B02 préexistante réutilisée sans modification/rebuild : source
**3f803d77393c6853ed76d796367df443a0eb9eb9**, 53312B, SHA
`59a236ae0d8f3d8f6b492fc84914cbf33cc5eaa304aaf13eebdb4e222f41cffb`.
FS16/488µgLSB, SPI3 8MHz/SD4MHz,60s/USERunique, archiveprévue `B020005.TR2`.
UID001 vérifié puis flashstandard et readbackentier strictementidentique.
Aucun ancien test/build relancé ; imagepréparée à l'avance et validations conservées.

La512Go est encore auPC lors du flash : disqueUSB511868665856B/D:FAT32,
partition1048576+511867584512, anciensA1A..H/B010005 présents et B020005 absent.
Aprèsflash sanscarte, refusinitialisationSD attendu99/error2/sd_error2/WHO7B,
writerinactif/nonincertain/FIFOoff/CShaute/GDBdétaché, zéroécriture. AucunSTART,
pas de formatage/réparation/suppression ni002/FRAM/COM4. Pas de fauxWAIT/READY.

**Transition physique demandée, encore en attente de retour :** éjecter512GoD:,
couperCN15+STLINK001/attendre10s, insérer512Go sur001, garderJP6USB/JP2retiré,
alimenterCN15secteur5Vseul/STLINKoff. AttendreWAITvert, ne pas presserUSERavant signal.
Le contrôlelibre peut durer plusieursminutes. Après60s seulement : maintienduCN15,
diagnosticretenu puiscopie/relecturePCintégrale ;10min seulement si conforme.

[Preuves B02 et transition](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/storage-B02-CN15/PREPARATION_ET_TRANSITION.md).
Journal actualisé avec firmwareactuelB02/absenceSDattendue, garanties et prochaineaction.

---

## Acquis D08 sous le firmware diagnostic D01


# D08 — minute autonome capteur/FIFO sous CN15 seul : PASS

**IN_PROGRESS : alimentation CN15 et exécution autonome démontrées sur cette minute ;
stockage encore à qualifier.** User confirme JP6USB/JP2retiré/STLINKoff/CN15secteur5V,
VDD/VDDIO3,315V, WAIT puis USER et vertfixe. Aucun debugger pendant la capture.

Snapshot récupéré sousSN001/UIDexact, **CRC b4bd1b0f valide**, différent deD07,
source `acb142794570ded5abd15f6521eb5d8b644590e8`, SHA
`5ec11cf108ce5c426ae3b3a388a916126f936a4627262f4c82849e613704ff27`.
BIN D01 de50912B entièrement relu identique. Aucun build, flash, reset ou STARTagent.
Première reconnexion dirigée au mauvaisbanc d'aprèsuser/énumération002seul ; aucun
accèsMCU002, logs conservés, puis branchement001 corrigé pour le relevé.

**PASS capturé :60,000079sMCU, stage4/error0,158576sortiescalculées,
598contrôles temporels/résidumax20µs≤100**,0FIFO/STOP/rails/clips/flags/queue/budgetmask,
STOPpropre133µs. FS16/SPI3 8MHz, FIR107Q15/dec10 et garde100µs inchangés.
Profil sansSD : aucun fichiercréé/aucunXYZintégralementarchivé, digest etsnapshot
sauvés ; writerinactif, SD0. Pas de qualification stockage/endurance/powerbank.

Étatcourant à la lecture : WAIT2/run1/error0/calibration33µs, contre calibration41µs
et résultatterminal danslesnapshot. **Nouveau démarrage observé entre essai et relevé**,
cohérent avec l'historique de reconnexionSTLINK, causeexclusive nonprouvée/CSRcumulé.
Le snapshot conservé permet bien de vérifier l'essai malgré les compteurscourants remis
àzéro. GDBdétaché/FIFOoff/SDCShaute ; ne pas relancer D08 ni appuyerUSER à nouveau.

Prochaine tranche autorisée : reprendre l'image stockage512Go B02 déjàpréparée,
60s/FS16/SPI3 8MHz/SD4MHz, nomneuf/CREATE_NEW, préflightCID/capacité/espaceinchangés,
puis relecturePCintégrale. Pas de formatage, pas de répétition des validationslogiciels
inchangées. AprèsPASS seulement,10min ; aucunlongSTART. Àcecheckpoint B02 nonflashée.

[Preuves D08 et limites](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/CN15-D08/D08_PASS_ET_SUITE_STOCKAGE.md).
HEAD initial `44c663a634868c78c492ae3d67ef3b8935077dc2`, journal actualisé avec
succèsCN15, observation de reboot/snapshotpréservé et prochaine étape stockage.

---

## Historique jusqu'au checkpoint44c663a6


# D07 — TR2-001 réussit 60 s après remplacement des câbles

**IN_PROGRESS, capteur/FIFO physiquement validé sur cette fenêtre sous ST-LINK.**
L'utilisateur confirme câbles remplacés sur001, moduleB conservé, puis vert fixe.
Il précise l'erreur passée après déplacement : CN8-6/8/10/12 au lieu de4/6/8/10,
cohérente avec WHO00 deD05 ; elle était corrigée avantD06, qui échouait encore.

PrécontrôleD07 : UID001/WHO7B, ±16g/FS04, SPI3 8MHz, calibration39µs≤100,
WAITsans erreur, BIN D01 de50912B entièrement relu identique à l'image archivée.
Source évaluée **acb142794570ded5abd15f6521eb5d8b644590e8**,
BIN SHA `6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2`.
Aucun build, flash ou reset ; un seul essai après les nouveaux câbles.

**PASS :60,000587sMCU, stage4/error0,158532sortiescalculées,599contrôles temporels,
résidumax24µs pour une limite100µs.** Zéro erreurFIFO/STOP, rail, saturation,
flagqualité ou débordement ; budgetmask0. FIFOarrêté proprement.
Snapshot67488B/CRC1779fd5f validé, source et SHA conservés dans les preuves.
**SDcommands/reads/writes0, initialized/write_enabledfalse**, writerinactif,
CS SDhaute, GDBdétaché. Aucune archiveSD créée, tousXYZnonpersistés : c'est un
diagnostic capteur/FIFO, pas une qualification du stockage. Aucun accès002/FRAM/COM4.

Le changement de câbles est associé à ce succès ; cela appuie la piste liaison/
contacts sans prouver une cause électrique exclusive ou identifier un fil défectueux.
Les anciens échecs restent conservés. Aucune nouvelle endurance ou powerbank qualifiée.

**Suite autorisée par l'utilisateur après le vert fixe : CN15 puis autonomie au
réducteur.** Même firmware D01 pour60s sousCN15 seul/secteur5V stable, sansSD,
afin d'isoler l'alimentation. Transition physique hors tension et USER parutilisateur.
Pas de répétition du succèsSTLINK inchangé. Si conforme, reprise de l'enregistrement
SD avec fichierneuf et relectureintégralePC, puis10min ; aucun8hSTART pour l'instant.

[Preuves D07 et plan CN15](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/last-cables-D07/D07_PASS_ET_SUITE_CN15.md).
HEAD initial de tranche `6b20e0a489121e948c72295c3b84d9740674cd75`.
Journal actualisé : succès borné, limites et nouvelle poursuiteCN15 demandée.
Checkpoint `98fafa31b11c0a6149e27d0d88a050205af1b2f7` publié et relu : 24 fichiers identiques,
693 empreintes conformes, worktree propre. Voir `last-cables-D07/remote-verification.json`.

---

## Historique jusqu'au checkpoint6b20e0a4


# Contrôle des deux bancs — B/001 et A/002 identifiés, D06 refusé sur001

**IN_PROGRESS.** L'utilisateur signale une correction de montage, B surTR2-001,
A surTR2-002, deux ST-LINK connectés ; nature exacte de l'erreur non précisée.
Identités et images des deux cibles vérifiées séparément : D01/50912B sur001,
N8G/53032B sur002, chaque BIN relu intégralement à l'identique. Aucun flash/build.

**Lecture actuelle WHO_AM_I : 0x7B sur les deux bancs**, un appel ST de lecture par
cible, puis CTRL1_XL=0x00 sur les deux (avant réarmement001). Traces HAL0/valides ;
scratchRAM sauvé/restauré, aucun registre capteur écrit par ces contrôles.
Le WHO0 de l'ancien arrêt001 était donc un résultat mémorisé, pas la lecture actuelle.
Sur002, seulement ces lectures : aucune nouvelle qualification FIFO/60s du module A.

Après préservation, **un reset logiciel001 justifié** pour quitter le refusWHO
verrouillé ; aucun reset002. Calibration001 réussie : résidu24µs≤100,198brackets,
6574ancres, WHO7B, ±16g relu04/SPI3 8MHz, WAIT2/run1/error0.
Observateur armé, USER pressé/relâché confirméuser, puis rouge.

**D06 échoue après3,010048sMCU : stage99/error3**, refus du compteur temporel.
31contrôles,79256natifs/7926sortiescalculées ; dernierFIFO397142/précédentdirect389251,
lecturefautive17571384 dans bracket73530397–73530415µs. Octets SPI reçus
**38 1E 0C 01**, décodant exactement cette valeur ; HALstatus/error0 et transfertcomplet.
Résidu216624521µs recalculé à l'identique, garde100µs inchangée. SnapshotCRCdb4f4cc4
conforme, SHA `1898379fcb1222e56a44817398f8e4397b56d085d07766ef2f5cb568aa621ea1`.
Source réellement évaluée001 `acb142794570ded5abd15f6521eb5d8b644590e8`.

FIFOerror0/arrêtpropre, rails/clips/flags0, budgetmask0. **Zéro commande, lecture ou
écriture SD**, pilote noninitialisé/gatefermé, writerinactif/CShaute/GDBdétaché.
Aucune archiveXYZ complète créée ; les volumes sont calculés, seuls diagnostics et
digests conservés. Aucun nouveau reset après le défaut. 002 reste auWAIT N8G,
SDsanscommande, firmware inchangé ; aucun START/flash/reset002, aucuneFRAM/COM4.

La correction rétablit l'identification et la calibration001 mais ne résout pas
le refus pendant l'acquisition. B passe sur002 et échoue encore sur001 : remplacer
le module seul ne suffit pas. Pas de preuve exclusive de MCU défectueux ou de cause
électrique, ni de qualification stockage/endurance. Pas de relance identique.
Prochaine étape proposée : remplacer les fils capteur001 par des fils courts du
stockutilisateur, sans dépouiller002, tout le reste conservé et hors tension.
Cette action physique n'est pas encore réalisée. Aucun8hREADY/longSTART.

[Preuves et limites](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/dual-module-recheck/RESULTATS_D06_ET_CONTROLE_DOUBLE.md).
HEAD initial `1326e3953e9c0516740c443fd4b6c4d299036d6d`. Tranche observation/debug,
sans code modifié : aucune compilation ou suite de tests inchangés relancée.
Journal actualisé avec les deux lectures, le reset justifié, D06 et l'état final.
Checkpoint `504408b01c00228d662549b814f4cf356bc44d73` publié et relu : 63 fichiers identiques, 671 empreintes conformes, worktree propre.
Preuve : `dual-module-recheck/remote-verification.json`.

---

## Historique jusqu'au checkpoint1326e395


# TR2-001 — D05 après déplacement du module B sur breadboard

**IN_PROGRESS, attente de mesures physiques.** L'utilisateur préfère préserver
le câblage éprouvé de TR2-002. Il confirme avoir déplacé uniquement le module B
sur breadboard, hors tension, en conservant les fils ; lecteur SD et jumpers
inchangés, puis TR2-001 alimenté par ST-LINK. Aucun fil neuf ou provenant de002
n'a donc été testé. Les mesures3,315V de D04 précèdent ce déplacement.

Contrôle sans build, flash, reset ou START : UID001 exact, même BIN D01 de50912B
relu intégralement à l'identique, source `acb142794570ded5abd15f6521eb5d8b644590e8`,
SHA-256 `6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2`.
**WHO_AM_I reçu =0x00, attendu0x7B**. Une transaction SPI conservée : registre0x0F,
longueur1, payload00, HALstatus/error0, RX/TXrestants0. Le succès HAL ne démontre
pas la qualité électrique de la liaison.

Arrêt99/error1/run0 avant réglage FS/calibration, zéro acquisition. FIFO jamais
démarré, writer inactif, CS SD haute, commandes/lectures/écrituresSD0,
pilote non initialisé/gate écriture fermé, GDB détaché. Aucun accès002/FRAM/COM4.
Pas d'appui USER demandé, ni de60s forcé, ni de test ancien répété.

Le défaut observé après déplacement diffère de D04 : l'identification échoue
désormais avant la calibration. Pas de preuve de module ou MCU défectueux.
**Prochaine action utilisateur demandée : mesurer VDD, VDDIO et GND du module par
rapport à GND Nucleo**, sans modifier le montage alimenté ; attendus3,3/3,3/0V.
Aucune nouvelle mesure n'est encore acquise. Le prochain contrôle dépend de ce retour.

[Preuves D05](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/breadboard-B/D05_RESULTAT.md).
HEAD initial de tranche `314ec12960635b814fe6bee3c5535fc59bc66d7c` ; documentation
et preuves seulement, pas de modification firmware. Journal actualisé.
Checkpoint `871ce9c01645d4f3685e01e03ef7c637d08a5205` publié sur main et relu : 18 fichiers identiques,
610 empreintes de preuves conformes, worktree propre. Voir `breadboard-B/remote-verification.json`.

---

## Historique jusqu'au checkpoint314ec129


# TR2-001 — module B éprouvé sur TR2-002, calibration toujours refusée

**IN_PROGRESS.** L'utilisateur confirme le transfert hors tension du module B,
celui des deux essais réussis sur TR2-002, vers le câblage conservé de TR2-001.
Module A mis à part ; VDD et VDDIO mesurés par l'utilisateur à **3,315 V**.

Contrôle D04 sans flash, reset ni START : UID001 conforme, BIN D01 de 50 912 octets
intégralement identique à l'image conservée, source **acb142794570ded5abd15f6521eb5d8b644590e8**,
SHA-256 `6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2`.
WHO_AM_I `0x7B`, réglage ±16 g relu `0x04`, SPI3 8 MHz. Le démarrage est déjà
arrêté sur **stage99/error7/run0 : refus de calibration**, avant le WAIT.
**Aucun essai de 60 s lancé et aucun appui USER demandé.**

Les 198 brackets de calibration sont sauvegardés : quatre lectures incohérentes,
résidu maximal **213 382 894 µs**, recalculé exactement hors ligne, limite inchangée
à 100 µs. Exemple : raw16 929 951 entre des voisins108 403 et110 008.
Les octets SPI originaux de ces lectures ne sont pas disponibles : la trace est
réinitialisée avant STOP dans calibrate(). Les valeurs sauvegardées démontrent
l'incohérence, sans déterminer sa cause électrique ou logicielle exclusive.

FIFO arrêté proprement, zéro erreur FIFO, writer inactif, CS SD haute,
**zéro commande/lecture/écriture SD**, pilote non initialisé et gate écriture fermé.
GDB détaché après préservation. Aucun accès TR2-002 dans cette tranche ; sa dernière
restauration reste acquise. Aucun changement FRAM, COM4, firmware, protections ou SD.
Aucun build ni test ancien relancé : tranche d'observation et d'analyse sans code modifié.

**Le module B fonctionne sur002 mais ne suffit pas à faire fonctionner001.**
Cela ne permet pas d'accuser le seul module A. Il reste à isoler fils/contacts,
alimentation effective et Nucleo001, sans prétendre que son MCU est défectueux.
L'ancien refus d'écriture SD est un problème distinct, non résolu par ce diagnostic.

Prochaine étape proposée : garder B sur001 et remplacer uniquement la liaison
capteur par les fils qui ont servi aux essais réussis sur002, hors tension,
sans modifier les réglages, le lecteur SD ou les jumpers. Action physique utilisateur
encore nécessaire ; aucun nouveau départ tant que le précontrôle n'est pas conforme.

[Preuves D04 et limites](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/module-cross/D04_RESULTAT_ET_SUITE.md).
HEAD de début de tranche : `c56872e1f0cc0cb83cf7d7269d999c702bf4c000`.
Journal actualisé avec le transfert rapporté, le refus, les preuves et la suite.
Checkpoint `3631037dc3604d79a6b479d61ff7ba64a4824680` publié par push normal sur main
et relu : 19 fichiers identiques, 594 empreintes de preuves contrôlées, worktree propre.
Relecture consignée dans `module-cross/remote-verification.json`.

---

## Historique jusqu'au checkpoint c56872e1


# TR2-001 — comparaison physique sans SD sur TR2-002

**IN_PROGRESS : comparaison terminée, localisation du défaut à poursuivre.**
L’utilisateur a explicitement autorisé cette extension le 02/11/2026 et confirmé
les deux bancs connectés en ST-LINK, avec la SD de TR2-002 insérée.
La décision est consignée dans les preuves ; le contrat ChatGPT reste inchangé.

| Diagnostic | Durée MCU | Sorties calculées | Contrôles temporels | Résidu maximal | Résultat |
|---|---:|---:|---:|---:|---|
| D01 / TR2-001, acquis | 16,659203 s | 44 288 | 167 | 228 049 169 µs | Refus error3 |
| D02 / TR2-002 | 60,000645 s | 158 450 | 599 | 19 µs | PASS borné |
| D03 / TR2-002 | 60,000639 s | 158 438 | 599 | 19 µs | PASS borné |

Sur TR2-002, les deux essais respectent la garde temporelle de 100 µs, les budgets
inchangés et les contrôles FIFO : zéro erreur FIFO, arrêt propre, zéro rail,
saturation, débordement de file ou masque de dépassement budgétaire.
**Aucune commande, lecture ou écriture SD**, pilote non initialisé et écriture
interdite, avant et après chaque essai. Aucun fichier créé : les nombres ci-dessus
sont des sorties calculées, pas des archives XYZ intégralement conservées.
Les snapshots et les digests sont sauvegardés, leurs CRC vérifiés.

Le profil comparateur conserve FS ±16 g, SPI3 à 8 MHz et FIR107 Q15/décimation 10.
Seuls la sélection UID et le départ RAM dédié diffèrent du diagnostic D01 ;
le profil autonome 001 conserve son appui USER obligatoire. GDB est armé au WAIT,
puis le CPU tourne sans pause jusqu’au breakpoint terminal après STOP/snapshot.
Deux essais au total, avec un reset logiciel entre eux pour vérifier la répétabilité.

Source évaluée : **6be825d4681a17224000ae1f2b088a315ba328c0**.
BIN : 50 928 octets, SHA-256
`7886feb2371837ceef7e126f26b4dedb2d484538a6142b8df2ccd662cc0dfbaf`.
Builds ARM comparateur et profil normal réussis ; configurations sans diagnostic
sans SD ou dépassant 60 s refusées. Les symboles d’initialisation, montage,
préflight et formatage SD sont absents de l’image liée. Pas de relance des anciennes
suites hôte inchangées. Le vérificateur de logs a d’abord manqué un retour à la ligne
CMake ; les refus effectifs ont été confirmés sans modification des contrôles.

**Préservation vérifiée :** identité SN/UID de TR2-002 contrôlée, deux sauvegardes
Flash complètes identiques et RAM initiale conservée. Image initiale N8G reconnue,
source `fbbdde5d328de47922cba688b6e406a327eb13f9`, 53 032 octets. Après les essais,
elle a été restaurée par flash standard. Le readback des **2 097 152 octets** de Flash
est strictement identique à la sauvegarde initiale, SHA-256
`113f54b740a2ee2be789ac42649becb8fe58b8a47d33890c587faabf405386e5`.
État final TR2-002 : N8G au WAIT, stage2/run0/error0, FIFO et writer arrêtés,
zéro commande SD, GDB détaché. Cette restauration ne requalifie pas N8G.
TR2-001 est inchangé : BIN D01 intégralement identique, ancien état99/error3,
FIFO et writer arrêtés ; aucun flash, reset ou START sur 001 durant la comparaison.
Aucune intervention FRAM, COM4, formatage, Option Bytes ou protection Windows.

**Conclusion limitée :** le défaut de compteur de TR2-001 n’est pas reproduit sur
TR2-002 pendant deux fenêtres de 60 s. La carte SD et son trafic ne sont pas
nécessaires à l’échec de TR2-001. Cela ne prouve ni une panne du MCU001, ni
l’innocuité absolue du logiciel partagé : capteurs, fils et conditions des deux
montages n’ont pas encore été croisés. Aucun signal électrique n’a été mesuré.
Aucune qualification d’endurance ou de stockage n’est acquise par ces diagnostics.

Prochaine étape proposée : échange croisé des modules capteurs, les deux bancs
hors tension, en conservant les câblages propres aux Nucleo ; puis un diagnostic
court sans SD sur 001. Cette intervention physique reste à effectuer par
l’utilisateur. Aucun troisième essai 002 ni départ 8 h dans cette tranche.

[Résultats détaillés et preuves](preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/bank-comparison/RESULTATS_ET_SUITE.md).
HEAD initial de tranche : `663df57b`. Journal actualisé avec les résultats,
les limites, la restauration et la prochaine action. Commit et push normaux sur
main réalisés : checkpoint `1195af35571e3b9d4041894bcc70e5ef57e71d18` publié et relu depuis origin/main,
83 fichiers strictement identiques, 577 empreintes de preuves contrôlées,
CRC et source des deux snapshots distants vérifiés. Worktree propre à ce contrôle.
Preuve : `bank-comparison/remote-comparison-verification.json`.

---

## Historique des acquis jusqu’au checkpoint663df57b (états précédents)


# TR2-001 autonome — reprise de la qualification progressive des alimentations

**IN_PROGRESS : essais repris sur accord explicite utilisateur du 02/11/2026**
(« OK nous sommes alignés, tu peux débuter quand tu veux »), après discussion du plan.
La campagne autonome reste non qualifiée. L'analyse A1H et ses preuves restent
acquises ci-dessous ; la suspension qu'elle décrit est historique, levée par cet accord.

Plan : 2×60s sous ST-LINK seul, 1×60s CN15+ST-LINK avec GDB,
1×60s CN15+ST-LINK sans attachement GDB durant le démarrage,
1×60s CN15 seul, puis10min sur la configuration finale seulement si tout passe.
Arrêt au premier échec ; pas d'acquisition longue8h. Accord et périmètre consignés
sans modifier MISSION_EN_COURS.md dans
`preuves/TR2-20261101-001-AUTONOME/qualification-alimentation-20261102/DECISION_ET_PLAN.md`.

**Diagnostic D01 sans SD : même erreur de compteur reproduite.** À la demande de
poursuite d'analyse et après proposition utilisateur d'échange de carte/banc,
préparation puis essai borné sur001 uniquement. Aucun accès à002/sa carte32Go.
Profil explicite `R3_TR2_001_SENSOR_ONLY`, OFFpar défaut, limité001/60s, excluant
l'initialisation SD et utilisant la session existante non-writer/run2. PiloteSD
noninitialisé et gateécriturefermée ; invariantzérocommande/lecture/écriture vérifié
avant départ et après arrêt. Aucun contrôle du compteur ou des budgets affaibli.
Cross-build du mode et du profil normal PASS ; configurations >60s/autrecible refusées.

Source **acb142794570ded5abd15f6521eb5d8b644590e8**. Douzième flash standard66,
BIN50912B/SHA **6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2**,
readback67 entier exact. WHO7B/FS04/calibration54µs/WAIT, capteur8MHz. USERphysique,
ST-LINKseul/GDBarméavantdépart, carte512auPC/lecteurTR2vide, CN15absent/JP6STLK/JP2off.
Breakpoint68 terminal seulement, après STOP/snapshot ; GDBdétaché puis lecture69.

Arrêt **16,659203sMCU**,99/error3,167checks, FIFOerror0/stoppedtrue, maskbudget0.
DernierFIFO **1489062**, précédentdirect **1481141**, lecturefautive **19689725** dans
le bracket **178243551–178243569µs**. Trace reçue **FD 70 2C 01**, HAL0/transfertcomplet.
Recalcul du résidu **228049169µs** exact/limite100. SnapshotCRC923a838d conforme,
SHA **06567836ce84183315691b83e06b0a116e5aa2ec40ba33d5b25fd13fcf5a9ed0**.
Lecture69 : **SDcommands=reads=writes=0, initialized=false, write_enabled=false**,
sensor_only=1, writerinactif. Maxima polls complets : service702,525µs/2500,
traitement373,16875/1000, intervalle962/5000 ; appel writer vide1,0125µs, pas transfertSD.
442872natifs acceptés/44288sorties calculées, digestRAM conservé ; **aucune archive
complète XYZ créée ou récupérée pour ce diagnostic sans stockage**.

**La carte SD et son trafic ne sont donc pas nécessaires à cet échec de compteur.**
Cela ne qualifie pas la512Go globalement ni ne résout le refusSD distinctA1D.
Le breakout SD/fils/alimentation restent présents, aucune forme d'onde capturée :
capteur, liaison, Nucleo et logiciel de transport ne sont pas départagés. Le driver
et le format32bits sont conformes aux référencesST consultées ; pas de correction
logicielle de bits ni de prétention de cause exclusive. Voir les preuves
`sensor-isolation/RESULTAT_ET_COMPARAISON_PROPOSEE.md` et `D01-failure-analysis.json`.

État final observé : 001 sousST-LINK seul, source arrêtée, GDBdétaché, lecteurvide,
SD512auPC ; firmwareD01diagnostic enFlash. Pas B02/CN15/10min. Le testcomparatif002
est maintenant pertinent, mais **le contrat l'interdit explicitement** : aucun flash,
debug ou geste sur002 sans extension humaine de périmètre. Plan borné proposé et
limites consignés dans la preuve précitée ; pas une qualification générale du firmware.

**Palier1 : B01 en échec après 17,088670 s MCU, sous ST-LINK seul.** L'observateur
est armé au WAIT (WHO7B/FS16, calibration16µs, capteur8MHz/SD4MHz), puis USER est
pressé physiquement. Le processeur tourne sans halt pendant la capture ; breakpoint
matériel atteint uniquement à `r3_finished`, après STOP et snapshot. Pas de reset ni
reconnexion du câble pour récupérer le résultat. Logs63/64 conservés.

Le snapshot67488B est valide, CRCc25b500d/source3f803d77, SHA
**c93226441712b19dfe8cdaa2f0141c5c67e19607104e8e18b132bf3cb2628492**.
`stage99/error3` : refus de cohérence du compteur. Dernier FIFO **1523255**,
précédent compteur direct **1515331**, lecture fautive **19823754** dans le bracket
**1049006171–1049006189µs**. La trace originale préservée avant tout reset contient
les octets reçus **8A 7C 2E 01**, qui décodent exactement cette valeur en little-endian.
La conversion numérique n'invente donc pas ce saut. HALstatus/error0, RX/TXrestants0,
cinq octets reçus incluant la commande, CSbas au contrôle avant fermeture ; ceci
ne constitue pas une mesure de forme d'onde ni une cause électrique exclusive.
Le recalcul indépendant reproduit **229404244µs** de résidu/limite100 inchangée.

Masque budget **0**. Maxima enregistrés des polls complets : service1446,79375µs/2500,
traitement720,75625/1000, intervalle1706/5000, SD456,39375/1000. Le passage fautif
renvoie sur l'horloge avant le calcul final de ses budgets ; aucune valeur manquante
n'est inventée. SDerror0/FIFOerror0/rails-clips0 ; STOP134µs, sourcearrêtée,
writerterminé/nonincertain/CS_SDhaute. GDBdétaché. L'échec de compteur ne nécessite
**ni CN15 ni l'absence de GDB** : l'hypothèse d'un défaut propre à CN15 est insuffisante.
Cela n'explique pas rétroactivement le refusSD A1D ou le dépassement de budget A1H.

Utilisateur transfère la512Go auPC et laisse001hors tension. Copie native complète
B010005.TR2, 2Mio, SHA **944b50e38613f7d30fc345fb1f71605fbec62faa6cd6197b07ca9f192a7c48d7**,
source=copie ; A1A–A1H re-hachés inchangés. Lecteur strict : INCOMPLETE576records/44928XYZ.
Ajout6recordsRAM vérifiés et30XYZpartiels : **45426sorties272556B**, SHA
**d1badcb64b64be828c97eaba5b03cf0f0e091168afce44329f9cbc498edc78a5**,
égal au digest producteur. Toutes les sorties filtrées produites récupérées, pas60s
ni tous les mots natifs. Preuves compactes, trace SPI active et suffixes publiables
sous `qualification-alimentation-20261102/`, originaux locaux dans le dossier homonyme.

**Arrêt de la comparaison au premier échec, conformément au plan.** B02 reste préparé,
non flashé/non lancé ; nouveaux paliersCN15 et10min non lancés. FirmwareB01 laissé
sur001 hors tension, JP6STLK/JP2retiré/CN15débranché, SD512 auPC selon utilisateur.
Étape suivante : traiter la lecture directe du compteur avant de reprendre les
comparaisons d'alimentation ; aucune garde supprimée/assouplie et aucune variante
à vitesses mixtes implémentée à ce point. Mission globale IN_PROGRESS, campagne
non qualifiée. Les états H/préparation ci-dessous sont historiques.

**Préparation logicielle de B01/B02** : source
**3f803d77393c6853ed76d796367df443a0eb9eb9**, diagnostic exact du budget temporel
(masque service=1/traitement=2/intervalle=4/SD=8, valeurs fautives et maxima conservés).
Expression OR de refus et seuils d'origine inchangés ; copie supplémentaire en RAM
après STOP. Test hôte ciblé frontières/violations combinées PASS, ARM ciblé PASS ;
aucune suite inchangée rejouée. Base commune de fréquences : **capteur8MHz/SD4MHz**,
comme l'essai A1C réussi ; FS16, filtre et garde horloge inchangés.

B01 et B02 : 60s USER, fichiers neufs B010005.TR2/B020005.TR2,
BIN53312B chacun, strictement identiques après remplacement du seul préfixe.
B01 SHA **7d9f2b59c312407273dc08ba8daa1e4758339259996beb1321b73aede66888d0** ;
B02 SHA **59a236ae0d8f3d8f6b492fc84914cbf33cc5eaa304aaf13eebdb4e222f41cffb**.
Préflight natif SD512/FAT32/cluster32Kio et absence des deux noms vérifiés en lecture
seule. Préparation physique demandée : SD auPC, lecteurTR2vide, CN15 débranché,
JP6STLK1–2/JP2retiré, ST-LINK seul. B01 a depuis échoué ; voir le résultat en tête.
Les informations de suspension/état matériel A1H ci-dessous sont celles du point
précédent, pas un état imposé après cette nouvelle intervention.

## 1. Résultat précis A1H

Snapshot réel de 67 440 octets conservé à travers le redémarrage observé lors de la
reconnexion ST-LINK : CRC **3ea13dd5** recalculé conforme, SHA
**461681043376180a6a522be624dd04e3b7987e2d69ea2c4d6be4dbbb0edff9a0**,
provenance source **65703ced5354d37724b2005bc0eba0fdad58d7dc**. Le relevé 59 distingue
l'ancienne capture du nouveau BOOT ; il ne transforme pas les compteurs remis à zéro
en résultat de l'essai. Voir `a1h-real-validation.json` et le journal GDB 59.

- `stage=99`, `error=6`, durée **0,279495 s MCU**, 7 032 natifs acceptés,
  704 sorties filtrées, arrêt FIFO confirmé ; aucune saturation ni clipping.
- Le code `poll()` renvoie -6 si service > 2 500 µs, intervalle > 5 000 µs,
  traitement consommateur > 1 000 µs ou quantum SD > 1 000 µs.
- Maximum SD retenu : **83 847 cycles / 160 = 524,04375 µs**. Cette garde SD
  n'est donc pas celle dépassée. Les maxima/dernières valeurs des trois autres
  budgets ne sont pas conservés : **impossible de distinguer service, intervalle
  ou traitement à partir de ce snapshot**. Aucun seuil n'a été relevé.
- Le contrôle d'horloge a effectué trois vérifications, avec écart maximal
  **63 µs / limite 100 µs**, mapper sans faute. Cela ne qualifie pas la fiabilité
  du compteur à 4 MHz sur 60 secondes.
- SD et writer : erreur 0, groupe terminé, réponse **0xE5 acceptée** (bits bas 5),
  aucune incertitude. FIFO : erreur/erreur STOP 0, arrêté ; niveau maximal 149,
  aucune file pleine. La vitesse réduite n'a donc pas réussi le critère court,
  mais aucune cause électrique exclusive n'est établie.

Analyse structurée : `a1h-failure-analysis.json`. Le matériel reste protégé par
les gardes ; pas de faux footer COMPLETE, de retry aveugle ou de correction de bits.

## 2. Données récupérées et préservation

Utilisateur : 512 Go transférée au PC, TR2-001 hors tension. Lecteur identifié
par capacité exacte **511 868 665 856 octets**, partition MBR/FAT32 vérifiée ;
la lettre D: relevée ce jour n'est pas un identifiant permanent. Copie native
intégrale `A1H0005.TR2` de 2 Mio, SHA source = copie :
**97b90e1b77b2f9e38adaacfb4a41ae32efea21ffe7c7a64023b472eaf40eba6f**.
Les fichiers A1A à A1G ont été re-hachés et sont inchangés. Aucune réparation,
aucun formatage ni écriture volontaire sur les fichiers source.

Lecteur strict : **INCOMPLETE**, huit records CRC/session/séquence conformes,
624 XYZ persistés. Ajout d'un record RAM également contrôlé et de deux XYZ
partiels : **704 XYZ / 4 224 octets**, SHA
**50be04e0308460ae21e86dd2cef3c6901a643963007097307da6f3ed4919b1e6**,
égal au digest producteur sauvegardé. Toutes les sorties filtrées produites de
cet essai sont récupérées ; cela ne signifie ni 60 s, ni tous les mots FIFO natifs,
ni durée SI indépendante, ni fichier COMPLETE.

Originaux et RAW dérivés :
`/home/lolo/dev/msm/reports/TR2-001-autonome-reducteur-20261101/`.
Preuves Git : `preuves/TR2-20261101-001-AUTONOME/` ; extents utiles compressés,
suffixes RAM, snapshots, copies natives, lecteur strict et SHA256SUMS.
La queue préallouée inutilisée est exclue des extents publics : leur reconstruction
est dérivée, ce n'est pas une copie intégrale originale dont on inventerait le hash.

## 3. Synthèse des essais acquis — aucune répétition des réussites

| Essai | Configuration / observation | Résultat réellement établi |
|---|---|---|
| A1A | Gate USER 3 s, SPI3 8 MHz | COMPLETE, 7 958 XYZ, relecture PC et digest amont conformes |
| A1B | Powerbank seule, 60 s demandées | INCOMPLETE, préfixe PC 13 104 XYZ ; RAM perdue, cause initiale inconnue, alimentation ensuite mesurée à 0 V |
| A1C | ST-LINK, 60 s demandées | COMPLETE après 60,000418 s MCU, 159 686 XYZ, relecture PC/digest conformes |
| A1D | Secteur CN15 seul, SD 4 MHz | Refus SD_RESPONSE, état 6, après 10,804 s ; 28 758 sorties produites récupérées, digest conforme |
| A1E | Secteur seul, SD 2 MHz | INCOMPLETE, 27 456 XYZ persistés ; état terminal perdu au reset, pas de preuve de toutes les sorties produites |
| A1F | Snapshot conservé, SPI3 8 MHz / SD 2 MHz | Refus du suivi d'horloge à 11,271195 s ; 29 999 sorties produites récupérées, digest conforme |
| A1G | Compteur brut et mapper conservés, même vitesse | Refus du suivi d'horloge à 42,023136 s ; 111 917 sorties produites récupérées, digest conforme |
| A1H | Option SPI3 4 MHz existante / SD 2 MHz | Garde de budget à 0,279495 s ; 704 sorties produites récupérées, digest conforme |

Pour A1G, l'entrée incohérente est démontrée : FIFO brut **3 518 223**, précédent
compteur direct **3 510 360**, nouveau direct **23 748 123** dans le bracket MCU
**400 152 755–400 152 773 µs**. Avec l'état mapper retenu, le recalcul indépendant
prédit 653 206 637 µs et retrouve exactement **253 053 864 µs** de résidu rejeté.
Le rejet est correct pour cette observation ; la cause de l'entrée incohérente
(transport, lecture du compteur ou autre) n'est pas physiquement isolée.
Il ne faut pas remplacer rétrospectivement le refus SD réel A1D par cette autre cause.

## 4. Modifications, validations et limites

Profil expérimental TR2-001 isolé : FS physique ±16 g relu, échelle nominale
488 µg/LSB explicitement portée par le format v3 et le lecteur PC ; compatibilité
±8 g conservée. CID/capacité SD512 stricts, MBR/BPB vérifiés, FAT32 en clusters
32 Kio, libre contrôlé, noms neufs CREATE_NEW. USER avec relâchement préalable,
antirebond, maintien volontaire, départ unique ; WAIT, LED et arrêt autonomes.
Aucun START long envoyé par Codex. Le firmware de production longue n'est pas déployé.

Les snapshots de diagnostic sont en RAM NOLOAD hors BSS, avec provenance,
version/taille/CRC et publication du magic en dernier. Ils ont survécu à un test
synthétique de reset logiciel puis aux resets réellement observés sur F/G/H.
Aucune garantie après perte d'alimentation. Ils n'écrivent ni FRAM ni SD.

Les tests ciblés bouton, carte, lecteur ±16 g/compatibilité ±8 g, réponses SD et
CRC de rétention ont réussi ; builds ARM des profils modifiés réussis, compilation
du profil historique également contrôlée. Les sorties réelles sont conservées.
Le changement H est seulement l'option existante SPI3 4 MHz ; aucun test acquis
inchangé n'a été rejoué. Dernière analyse H : lecture RAM, copie native PC,
CRC/digests et inspection du code, **aucun nouveau flash/reset/essai**.
Le protocole de validation ciblée convient au harness expérimental ; la suite
complète inchangée n'aurait pas démontré une preuve matérielle supplémentaire.

Les échecs d'outillage sont conservés dans l'historique : build lancé avant fin de
configuration, indentation refusée puis corrigée, lecture d'adresse SPI3 erronée
au relevé 57 puis corrigée au 58 sans nouveau flash/reset. Aucune validation n'a
été obtenue en désactivant un contrôle. Les précédents checkpoints détaillés sont
préservés intégralement dans `HISTORIQUE_CHECKPOINTS_AVANT_ANALYSE_A1H.md`.

## 5. Alimentation, cible et état de reprise

Cible unique : **TR2-001**, ST-LINK **003500463235510B37333439**, UID
**004A003D/38425018/20333157**. Les deux sauvegardes Flash initiales de 2 Mio ont été
vérifiées avant le premier flash, SHA
**31926eb263db85917e7f4c04b95109c2fb174102100349a7da532ca3bd9807c5**.
Aucun mass erase, Option Bytes, FRAM, COM4 ni action sur TR2-002/sa carte32Go.

Pendant une transition ST-LINK, un nouveau flag PINRSTF a été mesuré sans nouveau
BOR ; JP2/T_NRST a ensuite été retiré hors tension par l'utilisateur. D'autres
reconnexions ont malgré tout été suivies d'un nouveau BOOT avec flag logiciel
présent ; leur cause exacte reste inconnue. Deux attachements GDB avec câbles
inchangés ont préservé les compteurs et flags. JP2 retiré ne constitue donc pas
une correction complète. La mesure utilisateur 3,23 V sans ST-LINK est ponctuelle
et n'exclut aucun transitoire. Le manuel ST confirme la fonction de JP2 :
https://www.st.com/resource/en/user_manual/dm00789949.pdf

**État final rapporté par l'utilisateur : TR2-001 hors tension, 512 Go dans le lecteur
PC, JP2 retiré, JP6 USB [7–8]. Firmware A1H laissé en Flash**, source
65703ced5354d37724b2005bc0eba0fdad58d7dc, 53 056 octets, SHA
**76241cc81a5a3e18144de40157ee9719112476f299392c652be74a0b1faf0c70** ;
readback intégral conforme au relevé 58, SPI3 CFG1=0x10070007/diviseur4.
Aucun debugger actif après détachement. Ne pas réutiliser les noms A1A à A1H.
Powerbank actuelle non qualifiée ; autonomie énergétique 8 h et acquisition
longue intégrée non démontrées. N8G de TR2-002 reste FAILED historiquement.

## 6. Git, publication et décision attendue

Worktree de mission : `/home/lolo/dev/msm/projects/MSM_vibe-TR2-001-autonome`,
branche locale `codex/tr2-001-autonome`, push autorisé vers `refs/heads/main`.
HEAD avant ce point : **7fa56863b62f1c4be9c7c5e4fddc6cbf45dda27e**, relu depuis
GitHub ; les preuves G publiques permettent aussi de reconstruire les sorties
récupérées et leur digest. Le rapport d'analyse a été publié au commit
**788b02428b054929ce3fc7fd78da82ce6b2a716d**, puis les 27 fichiers modifiés ont été
relus intégralement depuis la référence récupérée de GitHub : octets identiques,
394 empreintes de preuves conformes, CRC/provenance du snapshot vérifiés. La
reconstruction des 704 XYZ depuis les seules preuves distantes retrouve exactement
le digest producteur. Voir `remote-h-analysis-verification.json`. Le présent suivi
consigne cette vérification ; son SHA publié et son état Git seront donnés dans
la réponse Codex. Le statut de ce point historique restait IN_PROGRESS, avec essais suspendus pour discussion ; la reprise autorisée est décrite en tête du présent rapport.

Les anciens worktrees et le dépôt principal (HEAD82169af7) sont préservés.
Deux stashes toujours présents : 6251b988e8b20f97997a8f98fd26d4d35a45a1b2 et
f3b829e6188f491920edf74a5f9ed0f559d08b87. Aucun merge/rebase/reset destructif.

Journal actualisé et synthétisé pour cette mission ; corps historique de la
référence initiale conservé octet pour octet. Aucun autre code modifié à ce point.
Prochaine étape : examiner les idées de l'utilisateur avant tout nouveau changement
ou essai. L'option d'une lecture courte à vitesse distincte reste une **idée non
implémentée**, sans autorisation technique nouvelle déduite de ce rapport.
