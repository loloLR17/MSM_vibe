---
mission_id: TR2-20261014-P12H3H-SDMMC1-READONLY-FIRSTFAULT
status: IN_PROGRESS
base_ref: main
base_sha: 94fffb0177fbfb6b02731a31a6e2f2ad284bf50b
initial_head: be0864ae11447c544dfb6ee460c8e983aaca0e15
result_sha: 42e8dd236d883366cf37167f9f96293fe95166f4
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Checkpoint — A PASS borné, B chargé au WAIT, départ B attendu

**État actuel : essai A unique PASS physique borné ; B construit et chargé au WAIT, pas encore exécuté.** Aucun CRC reproduit dans A. Pas de qualification TR2 ou industrielle. Les paragraphes de préparation ci-dessous décrivent les checkpoints antérieurs ; les mises à jour RUN A et flash B donnent l'état actuel.

## Acquis et preuves

Code isolé dans `experiments/sdmmc1_firstfault/`, source communautaire propre épinglée `1e3c9183bad023ebf7d234134823d9270a0e8b64`. Le générateur produit un overlay HAL/LL et main hors USB/TinyUSB/FatFs, avec [diffs et provenance](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/overlay-provenance.json). Les hooks figent la première erreur en RAM avant effacement des flags, sans instrumentation des boucles FIFO ni modification du recovery original. Les éventuelles transactions internes de HAL_Init après capture restent celles du HAL original : le wrapper arrête la suite au retour, il ne change pas artificiellement cette séquence.

Boot WAIT sans accès SD, trigger RAM unique, UID/CID/capacité/broches/horloges/largeur/SSR gardés. A=1bit/12MHz ; CMD17 512 puis CMD18 8192 au LBA506304 uniquement après phases conformes. Comparaison complète embarquée et décodeur indépendant préparés. Aucun CMD13 ajouté après erreur. Collecte terminale seulement, pas de breakpoint pendant polling. B conditionné par PASS physique complet A.

[Build A attempt2](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/build-A-attempt2.log) : GCC ARM14.2.1, Os/g3, BIN21852octets SHA256 `fb2843742056093610f979268c004bb9c66455cf540069ea5d8f55531a252fb1`, ELF `1d88d7054a82ab75edb3bd7b6f6aa321e4095a3535dbba0107d1cd6cac285b01`. Builddir `/home/lolo/dev/msm/reports/SDMMC1-firstfault-A-build`, preuves locales `/home/lolo/dev/msm/reports/TR2-SDMMC1-FIRSTFAULT-20261014`. Images complètes conservées localement ; sources tiers non modifiées ni réembarquées.

Première tentative compile échouée : hook LL injecté dans des utilitaires DMA sans pointeur SDMMCx ; périmètre du générateur corrigé, [échec conservé](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/build-A-attempt1.log). Deuxième tentative réussie. Contrôles : profil implicite rejeté, py_compile des trois outils, ABI ARM2260/200/84octets vérifiée hors cible, décodeur testé sur frame valide et phase_count invalide. nm ne contient pas les symboles MSC/FatFs/write/erase listés dans artifacts-A.json. Ce contrôle statique ne prouve pas encore une exécution cible.

Validation proportionnée : build ARM du probe réellement concerné, garde de profil et outils hors ligne ; pas de répétition des campagnes ni validations globales de production sur code inchangé. Oracles SHA512 `a9f4d9b4604080bea7999abf2d2874653e71c43a7805bc1735068b1f004f3daf`, SHA8192 `2ac5183f9757f9b08e94e4cf2f4b1ee03a3a66171e8b366989e5b36b93ab563b`, issus du protocole antérieur, non nouvellement relus sur carte.

## Correction de repérage et sécurité

Main concurrent `e02f7c4d969dd94f7761b3c5c8332acbb50af7cb` suspend le matériel sur erreur de repérage alimentation. Manuel ST UM2861 table19 : CN11-16=3V3, CN11-18=5V, CN11-20=GND. Utilisateur corrige « bleu est sur CN11-20 et aubergine sur CN11-16 », puis confirme « ST-LINK déconnecté, bleu sur CN11-20, aubergine sur CN11-16, banc hor tension, SD card non insérée ». [Retours exacts](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/pinout-confirmation.json). Cela étaye le repérage hors tension ; aucune mesure de continuité, de décharge VDD ou inspection visuelle par Codex n'est prétendue. CN15 reste à confirmer explicitement déconnecté. Mission ChatGPT conservée sans modification.

Prochaine étape physique : ST-LINK seul, SD absente, CN15 déconnecté. Vérifier identité seconde cible et image originale avant flash qualifié A, readback et WAIT. Puis insertion de la carte de test hors tension et départ électrique A confirmé utilisateur, mesure VDD0 si possible ou limite documentée. Aucun essai tant que ces conditions ne sont pas réunies. Arrêt définitif du protocole en cas d'échec A ; pas de retry. Si A PASS complet, construire puis exécuter B après coupure utilisateur.

## Préservation et publication

Commit logiciel initial `42e8dd236d883366cf37167f9f96293fe95166f4` préservé. Main a avancé pendant la préparation ; utilisateur autorise explicitement l'intégration isolée. Worktree original de mission intact ; nouveau worktree `MSM_vibe-SDMMC1-firstfault-integrated` basé e02f7c4, réapplication du seul commit logiciel : `8d3db40267d4a1dc9093c9b0e3d9b31819768d69`, arbre source identique, build déjà acquis non répété. `result_sha` désigne le code au build initial ; commit réappliqué identique dans les sept fichiers du probe.

Dépôt principal historique propre `82169af7e76797731358852bec83bfb0ceb54ccf`, stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87` préservés. ROOTCAUSE-REVIEW archivée byte-identique depuis94fffb0. Journal actualisé : préparation logicielle, correction utilisateur et prochaine étape physique. Principal/FRAM/média512Go/Modbus/protections non accédés. Permissions exercées à ce checkpoint : logiciel et commits ; push normal prévu avec contrôle distant, aucune opération matérielle.

Statut IN_PROGRESS de reprise, pas DONE. Publication et relecture distante de ce checkpoint à effectuer ; SHA publié communiqué après contrôle, sans auto-référence dans ce commit.

## Mise à jour — préparation matérielle accomplie, RUN A non lancé

Checkpoint12ca34935468afe09e0951cea055d1ea197976a6 publié normalement et récupéré de GitHub :25fichiers relus byte-identiques,13empreintes vérifiées ; [preuve](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/remote-readback-12ca349.json). Les avertissements whitespace concernent les diff tiers et CRLF des logs bruts, conservés intacts ; documents/JSON contrôlés.

Après confirmation utilisateur « Je confirme ST-LINK connecté, SD card retirée, USB C débranché », préflight par GDB attach sans reset : UIDconforme, image originale31437octets SHA2a2fe744… identique et sauvegardée. Le firmware précédent était dans Error_Handler sans carte ; cela ne constitue pas un nouvel essai SD. Flash standard qualifié secondeSN uniquement, secteurs nécessaires0–2, verifyOK/resetlogiciel avant insertion. Readback21852octets exact SHAfb284374…, observation WAIT1/trigger0/run_count0 puis detach. [Synthèse et logs bruts](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/flash-A-summary.json). Pas de mass erase/OptionBytes/RDP, pas de commande SD opérateur.

Autorisations flash/debug exercées pour cette préparation, après résolution du repérage documentaire et confirmation utilisateur hors tension. Mission active inchangée. Le seul RUN A reste à effectuer après coupure/insertion/reconnexion confirmées ; mesureVDD0 si possible, sinon limite explicite. Carte test absente lors du flash ; état au moment d'insertion non présumé. Pas de restauration automatique du firmware USB original afin d'éviter des transactions SD/MSC non prévues.

## RUN A — PASS physique borné, profil B construit ensuite

Départ confirmé utilisateur : ST-LINK seul/carte insérée/CN15off, VDDbreakout **0,003V mesuré lors de l'insertion**. Mesure utilisateur, pas mesure automatisée. Condition de sécurité documentaire/repérage hors tension satisfaite, fichier mission ChatGPT resté intact. Un seul trigger A, détachement avant polling, collecte terminale après10s sans reset ni breakpoint en transfert. [Traces et contrôle complet](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/RUN-A-decoded.json).

A terminal100/raison0/run_count1, UID et CID exacts,62333952secteurs512, SSRwidth0/CLKCR2 et kernelHSI48 ⇒1bit/12MHz calculés, CPU160MHz/IPSR0. Init et lectures HAL_OK/ErrorCode0, première erreur valid0. CMD17/512 et CMD18/8192 au LBA506304 :0différence embarquée et comparaison Python intégrale, SHA512a9f4d9b…/SHA81922ac5183f… exacts. CMD13 init et postlectures retournentTRANSFER4/R1=900 ; CMD12 original après DATAEND du multibloc observé. HALContext1/2 reste historique après succès, sans l'assimiler à une carte encore DATA ; statut carte séparément observé. Durées DWT : lecture5120,965331ms, lecture819211,575350ms ; appels HAL instrumentés, pas benchmark industriel.

A ne sera pas rejoué. Succès borné horsUSB : il autoriseB mais ne démontre ni causeUSB ni fiabilité/endurance. B construit seulement aprèsPASS A, mêmes overlayHAL/LL/main byte-identiques et codeprobe inchangé, seule définitionFF_PROFILE=4. Build/symboles/hashesB publiés. Les outils d'observation/décodage ajoutent sélectionB avec gardePASS A et noms de preuves distincts ; py_compile réussi. Aucun test de production inchangé rejoué. B physique non encore exécuté ; prochain flash standard B au WAIT puis coupure/reconnexion utilisateur obligatoire avant son trigger unique.

## Profil B au WAIT — prochain geste requis

PublicationA667154ee041e3b52c11fd54661cd58add57f94ee relue :57fichiers byte-identiques/45empreintes. Source du probe inchangée depuis42e8dd2, outilsB au commit667154e. Bbuild21860octets SHA256fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829.

PréflightUID et imageA exacte, terminal100 confirmé avant flashB. Flash standardverifyOK/readbackexact, WAITprofile4/stage1/trigger0/run_count0, debuggerdétaché. [Synthèse](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/flash-B-summary.json). Carte restéeinsérée, aucun init/accèsSD du nouveaufirmware avanttrigger ; resetflash avant futur départélectriqueB, pas un RUN. En attente coupureSTLINK/VDD0 si possible/reconnexionSTLINKseul parutilisateur ; CN15 reste déconnecté. Un seul essaiB, sansretry ; sauvegardesA et imageoriginaleconservées, aucune restaurationUSB nonnécessaire.
