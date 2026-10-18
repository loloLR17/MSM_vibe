# R5-B2 — preuves physiques du18/10/2026UTC

Le vrai service Windows local (372 fichiers identiques au build dc773733) et le firmware R5-B inchangé ont été exécutés sur COM4. Contrôleur natif source7bf84b1, uniquement START5/12/run11 et STOP5/13, aucun chevauchement de maîtres. Aucun contournement de politique Windows.

`physical-summary.json` et `analyze.py` :824 ADU du service/206 B3,1033 ADU du maître explicite, CRC exacts. SQLite234 observations COM4 :64 initiales,88 courantes,28 pertes logiques sans snapshot,54 HELD ;141 lignes API corrélées. La base contient aussi294 observations COM99 des deux smokes, sans mesure et clairement identifiées. Aucun FC10 du service ; les identités absentes du fil restent null. Les561 tests/builds acquis ne sont pas répétés.

Flash M016 entière2Mio restaurée exacte c5bc…f5b26 ; FRAM complète2b501a…7dd84 exacte au modèle et relue après restauration sans rollback. Historique/tail immuables, journal78/root91/epoch5/run11/config3gen3. Capteur arrêté/WHO7B, COM fermé et GDB détaché. Aucun SD/HP/mass erase/Option Bytes, produit permanent ou GO industriel.

BUNDLE conserve les empreintes des originaux et de leurs copies zlib ; SHA256SUMS couvre les fichiers publiés. Les sources sous hardware et leurs logs décrivent les opérations réellement exécutées. Les traces Windows gardent leur encodage original. `browser-running-dom.html` est un DOM rendu par Edge headless depuis le flux physique ; pas un screenshot ou une observation humaine. Premier helper HTTP échoué conservé, collecte suivante réussie avec Proxy=null pour la boucle locale ; service inchangé. `correlated-example.json` relie un même observationId du brut à l'API, et une observation affichée au DOM par son heure de réception exacte.
