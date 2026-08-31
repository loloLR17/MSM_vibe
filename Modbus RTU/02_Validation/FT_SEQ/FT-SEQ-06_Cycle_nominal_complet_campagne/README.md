# FT-SEQ-06 — Cycle nominal complet de campagne

## 1. Objet

FT-SEQ-06 constitue l'essai système E2E principal de FT-SEQ. Il assemble dans une seule exécution logique les chaînes nominales validées précédemment : temps, configuration, démarrage, campagne, arrêt et consultation finale.

## 2. Scénario propriétaire

Un test est créé :
- `TT-SEQ-SYS-001` — Cycle nominal complet de campagne.

Chaîne de référence :

`contexte` → `temps préparé/synchronisé` → `configuration préparée/active` → `START` → `campagne en cours` → `STOP` → `campagne clôturée` → `consultation finale`.

## 3. Nature de l'ordre retenu

L'ordre ci-dessus est un **ordre de scénario de test reproductible**. Il ne devient pas automatiquement une exigence générale du protocole.

Ainsi, FT-SEQ-06 n'invente pas :
- de handshake initial obligatoire ;
- d'obligation de SYNC TIME avant chaque START ;
- d'ordre universel entre SYNC TIME et APPLY CONFIG ;
- d'obligation de consultation B7 après STOP.

Les seules dépendances donnant un verdict sont celles déjà explicitement normatives dans les chaînes composées.

## 4. Couverture

- `COVERED` propriétaire FT-SEQ : 2
- `CONDITIONAL` : 0
- `DELEGATED` : 6
- `TRACE_ONLY` : 0
- `NOT_DEFINED` : 5

Les deux exigences propriétaires sont :
- `SEQ06-R01` — cycle nominal complet ;
- `SEQ06-R10` — cohérence E2E sans renforcement des oracles.

## 5. Délégations principales

- qualification/contexte : FT-SEQ-01 ;
- préparation/activation configuration : FT-SEQ-02 ;
- préparation/synchronisation temps : FT-SEQ-03 ;
- START/ouverture campagne : FT-SEQ-04 ;
- STOP/clôture/consultation : FT-SEQ-05 ;
- oracles élémentaires : FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-INT, FT-CMD ;
- reboot : FT-PER ;
- robustesse hostile : FT-RBT.

## 6. Doctrine E2E

`TT-SEQ-SYS-001` n'est pas la concaténation exhaustive des tests précédents. Il sélectionne des jalons représentatifs et utilise réellement la sortie d'une phase comme contexte de la suivante.

Un échec doit être rattaché à son oracle propriétaire. Une propriété qui ne pourrait être démontrée qu'en inventant une égalité ou une tolérance est déclarée non concluable, jamais renforcée silencieusement.

## 7. Prochaine étape

Après validation de FT-SEQ-06, FT-SEQ-07 traitera les chaînes normatives de refus puis reprise, sans dupliquer les refus isolés déjà couverts par FT-CMD.

Voir `source/FT-SEQ-06_source.md` et `detaille/TT-SEQ-SYS-001.md`.