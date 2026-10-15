# R3-A — propriétaire continu et revue d'architecture

Implémentation logicielle suivant R0/R1/R2. Les classifications et tailles ci-dessous
sont des observations du code/build, pas une qualification industrielle.

## Chemin acquis et composition

`fifo_live.c` utilise le driver officiel ST inchangé et les fonctions de statut et
parsing **extraites sans changement de corps de fifo_window.c**. Le chemin borné
continue de les appeler, avec ses limites et oracles historiques inchangés. Le
nouveau propriétaire conserve la continuité compteur/timestamps entre bursts,
mais pas l'intégralité d'une capture en RAM : tampon constant128×7=896octets.
Le callback reçoit seulement un burst entièrement contrôlé ; il emprunte les
bytes jusqu'au retour, ne bloque pas et ne fait aucune I/O média. R3-B devra copier
ou traiter ce burst avec le run/config/axes/échelle immuables du propriétaire.
Aucun RMS, fenêtre complète, qualité B3 ou résultat valide n'est fabriqué ici.

`live_fifo_acquisition` traduit ce propriétaire en vrai `LiveAcquisitionPort` :
START configure et relit ODR/FS/BDU/FIFO/timestamp ; observe relit les contrôles,
associe run_id/config_id/génération et ne déduit pas une confirmation d'un seul
write. STOP tente OFF/BYPASS/timestamp OFF et relit, même après une erreur antérieure,
sans dépendre de FRAM. STOP est idempotent et init ne touche aucun registre, ne
prétend pas que le capteur inconnu est arrêté. Un run tenté est consommé, sans
réutilisation dans cet objet ; la monotonie entre boots vient de R2 persistant.

Tous les axes matériels sont capturés en XYZ ; le masque logique1..7 est figé
par run et sera appliqué aux calculs R3-B. Les trois axes ne sont pas transformés
en un codage matériel fictif XY/XZ/YZ que le driver ne propose pas. Le parseur
hérité refuse conservativement une saturation de n'importe quel axe, même non
sélectionné : aucune valeur faussement valide, mais cette politique peut rejeter
une fenêtre que R3-B souhaiterait exploiter. À arbitrer/tester avec ses oracles.

`stm32_live_acquisition_attach/poll` réutilise le **même transport qualifié SPI3**,
avec garde UID TR2-001, horloges160MHz/HSI16, CS PC9 et prescaler4/8MHz existants,
contrôle de fin HAL et préclose. Le transport attach n'accepte qu'un propriétaire
par boot : le FIFO borné et le continu ne peuvent être attachés simultanément.
Pas de changement de pins, HAL, driver ST ou de fréquence par cette mission.
Les diagnostics bruts finis sont recyclés seulement au début d'un quantum dont
le précédent est vérifié ; les erreurs sticky ne sont jamais effacées. Le callback
de nettoyage conserve le budget réservé à l'arrêt en cas de défaut.

Le temps d'entrée est64bits. Sur STM32, l'extension HAL32 suppose le service à une
cadence inférieure à un tour de compteur ; le watchdog de gap protège une
acquisition en cours. Les tests franchissent la frontière32bits sans expiration
arbitraire du run. Un gap de poll, une absence prolongée de données, overflow,
parité/tag/compteur/timestamp, dérive de configuration, erreur SPI ou refus du
consommateur arrête le flux ; un burst rejeté n'est jamais publié partiellement.
Le seuil gap est une politique explicitement fournie, **pas une marge physique
mesurée**. La validation ne reconstitue jamais des échantillons perdus.

La composition de production reste V1 : ces nouvelles fonctions sont compilées
mais non appelées. Le lien ARM forcé prouve leur résolution avec le transport
réel ; il ne choisit pas ce propriétaire au boot. Le test d'intégration relie le
vrai moteur/store R2 au vrai propriétaire sur SPI simulé, sans faux observer.
R3-B devra choisir explicitement le propriétaire continu à la place de la capture
bornée, composer RTU2/B1/B3/B6/B7/effets hérités et garder un seul ordonnanceur.
Le callback budget FRAM reste absent/refusé par défaut : les commits synchrones
R2 après démarrage restent incompatibles avec une garantie FIFO non démontrée.
Ne pas annoncer LIVE/protocole2 avant composition et budgets cohérents.

## Cartographie et décisions de mutualisation

Inventaire fichier par fichier : `module-inventory.json` dans les preuves,
avec références CMake et chemins de symboles issus de `nm -l` de l'ELF canonique.
Un fichier compilé n'est pas nécessairement lié ; un module préparé non lié
n'est pas déclaré mort. Headers : interfaces, pas unités de linkage.

| Chemin / groupe | Classification et preuve | Décision |
|---|---|---|
| AcquisitionService + VibrationSource V1, SystemRuntime, production_application | Historique requis ; composition STM32/projection/PDU existante, symboles de l'ELF et tests V1 | Conservés, pas de réécriture de V1 |
| FIFO coopératif / fifo_window / HAL observé / transport ST | Liés au cumulatif de qualification ; configuration bornée/armement banc, chemins CMake conditionnels | Transport et parsing mutualisés, chemin borné conservé |
| R1 LiveConfiguration et média LIVE | Préparé/testé, symboles de bibliothèque sans appel dans production ; média transactionnel historique réellement utilisé | Partition disjointe et enveloppe communes, pas de deuxième driver FRAM |
| CommandEngine V1 / LiveCommandEngine R2 | Deux sémantiques explicites : V1 sans epoch et journal historique, R2 epoch/journal4/reçus ; tailles B5 distinctes | Conserver séparés tant que V1 doit fonctionner ; pas de facteur commun spéculatif |
| fifo_live + ports R3-A | Préparés, tests réels de service/store, lien forcé ARM ; absents du flux exécuté par main canonique | Réutilisés par R3-B ; ne pas les appeler code mort |
| Tests hôte/plateforme | Uniquement tests, CMake test_* ; modèles capteur/FRAM | Conservés, helpers historiques réutilisés sans recopier leurs suites |
| Autres profils experiments / harness SD / probes | Bancs historiques ou profils explicites séparés, preuves/recipes associées ; hors ELF canonique selon options | Conserver les preuves ; pas de retrait opportuniste |
| Modbus V1 / supervision PC S0–S7 | Protocole1 existant, modules PC hors métrique C/C++ firmware ; composition produit connue | Conservation ; décodeur et intégration LIVE distincts R3-B/R5 |

**Remplacé** : duplication potentielle du parseur et des guards FIFO, évitée par
export des fonctions existantes. **Différé** : migration de composition/retour à
un seul moteur produit après fin de compatibilité V1, selon décision future.
**Candidat à retrait immédiat : aucun démontré.** Réduction artificielle de LOC
et suppression des archives n'apporteraient aucune preuve de sûreté.

## Mesures reproductibles

`python3 'Modbus RTU/05_Firmware/tools/measure_live_architecture.py'` lit les blobs
Git pour b6bae880 (avant R1),3f3ca1c (HEAD R2) et les fichiers non ignorés du
worktree. Mesure physique `splitlines()`, commentaires/blancs compris : code
C/C++ propriétaire firmware **et candidats/expériences**, pas seulement lié ;
tests C/C++/Python/C#/shell ; documents MD/RST/ADOC. Vendor, générés, builds,
JSON/CSV/logs et données de preuve ne deviennent pas des LOC. Le script n'assimile
pas lignes ajoutées Git et taille finale. Le JSON fournit fichiers/lignes/nettes ;
les numstat Git sont conservés séparément, y compris les données non classées.
Les mesures finales ciblent le commit de code R3-A avant les commits d'échange.

| Référence | Code C/C++ propriétaire | Tests | Documentation |
|---|---:|---:|---:|
| Avant R1 b6bae880 |30415|45422|140951|
| HEAD R2 3f3ca1c |31768|46516|141714|
| Résultat R3-A (code, avant rapports ultérieurs) |31999|46620|142059|

La croissance réelle R1+R2 de cette méthode est1353lignes code,1094tests,
763documentation ; elle ne signifie pas1353lignes liées au produit. L'ajout des
archives/rapports n'est ni un driver supplémentaire ni du code mort. Les trois
colonnes ne comptent pas tout le dépôt ; outillage/autres données sont distincts.

Les tailles des images et les symboles sont dans les preuves. Le binaire canonique
et l'image de lien forcé sont distingués ; aucun des deux n'a été flashé. Le
propriétaire utilise une mémoire constante et des totaux64bits, sans heap ni
tampon dimensionné par la durée d'acquisition. Aucun budget de pile dynamique,
CPU sous charge ou latence bus physique n'est qualifié par des tailles statiques.

## Dettes limitées et prochaines preuves

1. **Risque fort, R4** : FRAM R2 synchrone après effet START, plus d'un Mo de relectures
   par commande ; garantir service FIFO/STOP avant autorisation réelle du budget.
2. **Risque fonctionnel, R3-B** : composition RTU2, effets hérités, fenêtres exactes,
   conversions/RMS/alarme/validité et snapshots B3 non implémentés ici.
3. **Risque matériel, qualification future** : comportement électrique SPI, seuil
   gap et coût des contrôles par quantum non mesurés sur cette carte ; aucune
   garantie d'endurance ou de zéro perte extrapolée des simulations.
4. **Conservatisme de données** : saturation sur axe non sélectionné et politique
   de consommation des bursts à préciser avec R3-B ; ne pas affaiblir l'oracle V1.

Aucune dépendance microSD, DataStore ou campaign_id dans le nouveau propriétaire.
L'absence d'une qualification physique n'empêche pas les preuves logicielles,
mais aucune capacité opérationnelle n'en est déduite.
