---
mission_id: TR2-20261016-R4Q-LIVE-B3-RTU-COMPOSITION
status: IN_PROGRESS
coverage: SOFTWARE_DONE_PHYSICAL_PARTIAL
base_ref: main
base_sha: 48805fe638151a33f2e44f17eb6da01a00e91efa
initial_head: cf9d1b5321923a0abb2494b46b2c7860e5dcecc6
result_sha: 6e813501a973fb25bd41bf3b0713435763414779
created_at_utc: 2026-10-16T15:04:00Z
updated_at_utc: 2026-10-16T15:04:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-Q — composition logicielle livrée, diagnostic cible readonly réussi

**Logiciel : DONE dans le périmètre incrémental B0/B3/B4/B5. Physique : PARTIAL.**
Le flux vrai propriétaire FIFO → métrologie R3-B → image RAM B3 → RTU est démontré
sur SPI/horloges simulés, avec vrais stores R1/R2 en charge. Les commandes sont
capturées en RAM puis traitées hors callback RTU/ISR, avec admission R2 inchangée.
Le probe cible a mesuré dix secondes de lectures FRAM et de RTU loopback RAM,
capteur arrêté et B3 sans valeur valide ; ce n'est pas B3 vibratoire physique.
La Flash initiale est restaurée, la FRAM R4-P epoch4/run4 reste entière inchangée.
IN_PROGRESS porte seulement sur la publication/relecture finale à obtenir.
Aucun blocage technique n'a empêché la livraison logicielle ; le temps événement
STM32 non qualifié limite la qualification physique complète, reportée explicitement.
Aucune décision normative inventée, aucune qualification R6/industrielle annoncée.

## Références, autorisations et préservation

HEAD initial main `cf9d1b5321923a0abb2494b46b2c7860e5dcecc6`, contrat R4-Q READY,
base `48805fe638151a33f2e44f17eb6da01a00e91efa` (R4-P DONE, jamais relancé).
AGENTS, gouvernance A-06/protocole, journal intégral/roadmap, R0 et interfaces R1–R4,
rapport/preuves R4-P consultés depuis Git réel. Aucun changement normatif ni dette A04.
Chemins Linux/SDK/scripts/backups vérifiés, aucune migration silencieuse.

Main local historique propre `82169af7e76797731358852bec83bfb0ceb54ccf` préservé,
sans pull risqué/merge/rebase/reset. Worktree isolé
`/home/lolo/dev/msm/projects/MSM_vibe-R4Q-live-b3-rtu`, branche
`codex/r4q-live-b3-rtu`, publication normale explicite `HEAD:refs/heads/main`.
56 anciens worktrees inchangés, deux stashes conservés :6251b988…a45a1b2 et
f3b829e6…8b87.2349fichiers protégés et3745anciennes preuves vérifiés byte-identiques.
Contrat et rapport R4-P archivés exactement depuis48805fe dans archives/
TR2-20261016-R4P-PHYSICAL-COEXISTENCE ; aucune archive antérieure réécrite.
Toutes permissions du contrat exercées : développement/tests, commit/push,
flash standard/debug qualifiés sur TR2-001. Ni SD, HP, RS485, OptionBytes/RDP,
mass erase, changement de boot, identité ou intervention physique.

## Analyse d'écarts et choix minimal

[Analyse et contrat d'usage LIVE_R4Q](../../05_Firmware/LIVE_R4Q.md) : le port FIFO
R3-A et son binding R3-B sont réutilisés ; aucun driver/parser/format R0/R1/R2
modifié. Le runtime RTU est réutilisé pour CRC/adressage/silences/broadcast/TX ;
seul ajout partagé : handler PDU optionnel, NULL conserve le chemin de traitement V1.
La production reste V1 ; le nouveau serveur est une composition partielle explicite,
pas un nouveau firmware produit LIVE annoncé.

Nouveau `LiveModbusServer` : B0 préparé protocole2 fourni par l'autorité, B3/B4/B5
publiés en RAM, deux buffers, propriétaire unique au premier plan. Traitement d'un
PDU sans yield/publication concurrente, copie de toute la réponse avant retour.
Aucune primitive multithread/ISR concurrente revendiquée. Cohérence **par ADU** ;
plusieurs lectures séparées peuvent observer une nouvelle séquence, pas de promesse
atomique inter-ADU ou inter-blocs. Pas d'adresse ajoutée pour run/config/génération :
contexte de la valeur conservé en RAM, B4/B5 gardent leurs autorités et layouts.
Une ancienne valeur retenue garde son contexte, pas celui du run suivant.

Âge et fraîcheur B3 évalués à chaque lecture sur copie RAM de métrologie dont les
callbacks sont supprimés. Lecture ne rajeunit pas la valeur et ne touche ni capteur
ni FRAM. UTC absente : V numérique possible/F selon âge, TIMEBASE_VALID0/timestamp0,
qualité dégradée visible. Recul du monotone : invalidation RAM immédiate, faute
sticky, arrêt réel au prochain owner_step ou par garde coopérative ; aucun arrêt
matériel depuis PDU. Le propriétaire publie après poll/invalidation/chaque quantum.

FC10 B4 réutilise le staging/codec existants, sans I/O ; APPLY capture le préparé
immuablement avant admission R2, puis revalide CRC/domaines et persiste avant active.
FC10 B5 acquitte **une capture RAM, pas RESERVED ni succès métier**. Trois captures
normales et une réserve STOP, plein→exception04 sans mutation de la requête.
Mailbox reflète le staging ; statut/last/epoch viennent du moteur R2 publié.
CLEAR ne supprime pas une capture immuable ; CANCEL/flags invalides restent remis
au vrai handler R2, sans ajouter d'effets ni de confirmations.

Owner_step traite au plus une capture/admission ou une phase durable. Guards
owner_busy/responding/depth/servicing interdisent toute réentrée depuis le service
coopératif. Alternance capture/phase assure le progrès sous trafic. STOP réservé
prioritaire lorsque R2 libre/LIVE actif, sans réordonner une collision de clé ;
jamais préemption d'une transaction durable admise. Refus R0 contre nonterminal
retournés par R2, jamais transformés en commandes à réessayer automatiquement.
Ce n'est pas une garantie anti-DoS : réserve finie, client doit gérer exception04,
et la durée transactionnelle STOP inclut toujours les barrières FRAM.

**Composition PARTIAL assumée :** B1/B2/B6/B7 et effets hérités2..11 restent à composer,
plages non implémentées refusées (exception02), aucune autorité fabriquée. B0 reste
préparé2/capacités0x000C, aucun bit LIVE opérationnel ajouté. Pas d'activation produit
avant ces dépendances. Aucun fournisseur de temps événement STM32 qualifié : le
callback par XYZ est obligatoire, seule une grille explicitement simulée est utilisée
par les tests. DWT/HAL est utilisé pour scheduler/âge, jamais déclaré temps capteur.
L'absence UTC ne bloquerait pas les valeurs numériques ; l'absence de temps événement
qualifié empêche d'en annoncer ici une fenêtre physique conforme.

## Validations effectives et erreurs conservées

Changement d'interface partagée/runtime : validation complète requise, exécutée par
`STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`.
**140 tests hôte PASS,61,23s ; cross-build canonique PASS.** Inclut les régressions
R0/R1/R2/R3/R4 et V1 ; cette répétition est justifiée par le changement du runtime.
Aucun test antérieur relancé physiquement. Ajout quatre tailles au nouveau test :
seul test140 relancé et PASS2,86s, sans répéter les139régressions inchangées.
Sanitizers `-fsanitize=address,undefined -fno-omit-frame-pointer -g -O1` ciblés
nouveau serveur/runtime :2suites PASS4,04s ; nouveau complément quatre fenêtres
seul PASS3,45s. Probe ARM dédié PASS ; profil incomplet R4Q explicitement refusé
par CMake, test négatif conservé. Warnings link newlib connus dans les logs,
pas d'erreur masquée ni de test désactivé. Le guard ajouté à main du test R4 sert
uniquement la réutilisation de sa fixture : sa régression reste enregistrée/exécutée.

Test140 : vrai propriétaire FIFO/driver ST derrière SPI simulé, vraie R3-B, vrais
stores R1/R2 avec quanta32octets. Modèle SPI1=2,5MHz/SPI3=4MHz, arrivée indépendante
30000,375mots/s, allowance CPU déclarée12+5us/XYZ, **pas mesure MCU** :

| Mesure modèle | Résultat |
|---|---:|
| XYZ traités / séquences |1554683 /12|
| Réponses RTU / B3 / B5 |7019 /4624 /2312|
| Travail / service / gap FIFO max |484 /498 /613us|
| FIFO max / backlog RX max |26 /6|
| Latence enqueue réponse depuis premier octet, maximum |24874us|

Aucun overflow de production modèle. Lecture48registres vérifiée entière contre
image sélectionnée sous publications répétées ; pas déchirure à l'intérieur d'un
ADU. N=4096/8192/16384/32768, oracle indépendant61mg constant, identité/génération,
UTC fiable/absente et fraîcheur au temps de lecture. Retries et collision27,
STOP réservé avec file normale pleine, admission différée/absence I/O en PDU,
APPLY immuable face à staging modifié, reboot et retry d'un START terminal sans
effet nouveau, panne FRAM et invalidation/arrêt, CRC mauvais, trame>256octets,
overrun UART simulé, déconnexion/reconnexion et erreurTX testés. Absence de FRAM
pendant chaque callback vérifiée par compteurs du vrai backend ; non-réentrée
vérifiée pendant les quanta. Les tests UART STM32 historiques restent dans les140,
mais leurs mocks ne deviennent pas des IRQ physiques mesurées.

Échecs conservés des nouvelles fixtures : attente initiale0 incorrecte après boot
achevé (état inactif1/V0/F0), nouvelle source RTU injectée au milieu d'une trame
(fixture réinitialisée au début), génération1 supposée malgré vraie application
successeur donnant2 (attendu fixé au contrat réel). Aucun oracle gelé ajusté.
Une lecture hors tableau d'un vecteur local de construction de test a été corrigée
avant sanitizer ; état final instrumenté PASS. Premier lancement direct du script
sans bit exécutable échoue126 ; exécution documentée via bash réussie, log conservé.

## Images, provenance et non-régression V1

Code réellement évalué : `6e813501a973fb25bd41bf3b0713435763414779`.
SDK Linux STM32CubeU5v1.9.0, GCC ARM14.2.1, Cortex-M33, Debug/recette CUMULATIVE,
manifest/registry M014-H inchangés ; options complètes dans build-artifacts.json.

| Image | Octets BIN | SHA256 BIN | SHA256 ELF |
|---|---:|---|---|
| Probe R4Q NON-PRODUCTION |136696|b44608297178a8c7cfc7566d8b3e2820a977448485408bf3bdb5b056112ba9f7|2c3298b4abc78c9d656a2c3673312367784abd91554fa789624cb48b4b06e2b8|
| Canonique V1, non flashé |173132|85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5|bd2dceaafdc570a3378945ea6c100d6c6605228db34224fdd8a1fd11c00a206b|

Le BIN canonique diffère de6a2150af…15a9 : runtime enrichi et disposition mémoire
modifiée. **Pas de non-régression bit-à-bit annoncée** ; démonstration adaptée par
140tests dont les comportements V1, handler NULL au boot et mêmes options produit.
La Flash M016 existante est conservée/restaurée exactement, pas remplacée par ce
canonique nouveau sans mission de qualification. Aucun changement de format ou
algorithme R2/R3-B/V1/driver ST. ARM probe text136204/data492/bss605032 ; serveur
RAM4344octets et métrologie720 selon symboles ELF. Pas de borne de pile ou CPU
sous charge complète déduite de ces tailles.

## Préflight et observations STM32

Unique ST-LINK SN003500463235510B37333439/V3J15M7, NUCLEO-U575ZI-Q,
UID004A003D/38425018/20333157 confirmés. Vérification fraîche14:54:46UTC :
Flash2097152 SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26 ;
FRAM262144 SHA a13b3401c5ec93ec278e0b35ad2ad1fe38f42755dcc93ef61ce1b29f09aa2ef9.
Identiques aux sauvegardes complètes déjà vérifiées R4-P ; réutilisation justifiée,
pas de sauvegardes à chaque accès. Capteur relu WHO7B/ODRoff/FIFObypass/TSoff.
Debug de lecture seul avant flash ; staging RAM seulement, aucune écriture NVM.

Image dédiée hashée, programmation qualifiée CLI2.23 SWD8000/SN/adresse0x08000000,
write/verify/reset standard puis reset logiciel GDB qualifié ; préfixe BIN relu exact.
Aucun HOTPLUG, effacement massif, OptionBytes ou nouveau provisioning. Entrée dédiée
R4Q, seul trigger9 ; helper R4P qualify(5) readonly sur état final exact, ownership0,
aucun owner_step/captureB5/START réel. Noms r4m/r4_result réutilisés dans les helpers,
seul r4q_result.stage10 marque la réussite de ce probe, pas legacy.stage4.

Lancement trigger9 **14:58:41UTC**, lecture terminale après fin autonome :

| Observation cible DWT/HCLK160MHz | Résultat |
|---|---:|
| Charge readonly/loopback |10000754us|
| Réponses RTU CRC conformes |872 (218 par bloc B0/B3/B4/B5)|
| Travail max |341us <=1000|
| Service max |1074us <=2500|
| Publication RAM max |378us|
| Poll RTU/PDU max, incluant son retour loopback |637us|
| Lecture FRAM max / octets compteur harness |132us /1453156|
| Captures / appels owner / writesFRAM |0 /0 /0|
| B0 version/capacités |2 /12 (préparées)|
| B3 statut/flags/séquence |1 /4097 /0 ; V0/F0, pas valeur calculée|
| FIFO XYZ/TS/polls |0 /0 /0 ; capteur arrêté|

No deadline/error, unexpected0 ; epoch4/previous3/gen7/flags0, dernier START4/4
COMPLETEDgen24/run4/config3 retrouvé sans replay. `max_gap_us=0` ne prouve rien
sur une FIFO inactive.637us est une durée CPU de poll/traitement avec transport
RAM et CRC, **pas** une latence sur fil RS485 ; aucune IRQ UART/TX observée.
Port/adaptateur/câblage RS485 actuel non qualifiés par cette mission, aucune absence
physique supposée. Pas de sollicitation utilisateur pour élargir ce sous-essai.

Lecture intégrale FRAM après probe identique à avant. Audit offline CRC racines,
header/payload/journal/epoch/TR2E/provenance conforme, racine37, pas d'indétermination.
Historique[0,122880), LIVE[122880,245760), tail[245760,262144) tous byte-identiques.
Aucune nouvelle transaction, epoch/run consommés inchangés ; rien à rembobiner.

## Restauration et limites

À15:00:46UTC : Flash initiale restaurée par programmation normale17pages/139264octets,
puis **2097152octets relus exacts** SHA c5bc344d…f5b26. FRAM encore relue entière
après restauration : exactea13b3401…a2ef9,0writeFRAM. M016 CommissioningReadOnly
au WAIT/GDB détaché, capteur arrêté ; aucun LIVE opérationnel laissé par supposition.
Sauvegardes récupérables sous `/home/lolo/dev/msm/reports/TR2-r4q/hardware/`,
anciennes R4-P préservées ; snapshots FRAM sans perte et empreintes publiés.

**Non démontré / suite précise :** fournisseur temps événement capteur observé/calibré
et correspondance UTC, acquisition physique+R3-B+RTU+FRAM simultanées, IRQ/TX/latence
électrique et déconnexion réelle, STOP/WCET sous panne SPI/IRQ et charge complète,
CPU/pile, composition B1/B2/B6/B7/effets hérités avant capacités LIVE, R5/PC/SQLite/Web,
endurance/zero perte globale/power-cycle/R6. Les maxima modèle et ceux du probe sans
FIFO ne s'additionnent pas pour qualifier ces scénarios. R4-P conservée comme preuve
bornée distincte, pas relancée ni étendue par déduction. Aucun seuil affaibli.
Recommandation à ChatGPT : contrôler ce lot logiciel et les preuves readonly puis
cadrer provider temps et composition restante avant campagne R6 ; aucune décision
normative nouvelle n'est nécessaire pour réceptionner la tranche préparatoire.

## Publications et état Git

Code6e813501…14779 et checkpoint32cc6a1a99bcbc95b3f9e7b3479a37d17b7c1c53 publiés
sur main et intégralement relus (66fichiers/52preuves) avant le probe. Le présent
bilan technique, preuves, journal et roadmap attendent leur publication/relecture
finale selon protocole ; statut DONE non anticipé. Aucun blocage de push connu.
[Preuves complètes](preuves/TR2-20261016-R4Q/README.md) : logs bruts, commandes GDB/flash,
images/provenance, tests avec échecs, snapshots zlib, audits et manifeste SHA256SUMS.
Journal actualisé sur état carte, portée logiciel/physique et suite ; roadmap R2/R3/R4
factuelle, sans fermeture de R4/R6 ni jalon indépendant. Contrôle indépendant ChatGPT
reste distinct du statut de livraison Codex.
