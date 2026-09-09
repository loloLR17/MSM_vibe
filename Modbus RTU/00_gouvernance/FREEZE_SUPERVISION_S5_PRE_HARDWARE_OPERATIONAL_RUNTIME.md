# Projet MSM — Capteur de vibration TR2

## Gel S5 — Exploitabilité PC pré-matériel et robustesse du runtime

Date de gel : 2026-09-09

Ce document clôture **S5 — Exploitabilité PC pré-matériel et robustesse du runtime**. Il complète les gels S0 à S4. Il ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Baseline logicielle validée

Dernier état logiciel S5 effectivement compilé et validé localement avant la clôture documentaire :

`fe02e4e89c562978d5489ae7e80b7e3defd0cb46`

`Supervision: test incoherent B5 startup recovery failure`

L'utilisateur a confirmé build et tests verts avec :

```bash
git pull --ff-only origin main
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```

Après cette validation logicielle, deux commits documentaires d'audit ont été ajoutés :

- `199a2d0d5394aae2ea5c74f9f2f33dfda6750c43` — `Supervision: align documented serial stop bits` ;
- `81e5adfa1f9daab3e1db6875af6bc94ade72642c` — `Supervision: update PC prerequisites for S5`.

Ils ne modifient aucun code de production ni test. Le présent document constitue le commit de gel S5 qui les suit.

## 2. Objet et frontière de S5

S5 transforme le runtime logiciel issu de S4 en un programme PC exploitable **avant disponibilité du matériel TR2/STM32/RS-485 réel**.

S5 couvre :

- démarrage robuste ;
- host console exécutable ;
- configuration opératoire explicite ;
- diagnostics locaux minimaux ;
- reprise durable B5 au redémarrage ;
- robustesse logicielle par stress ciblé ;
- audit documentaire de clôture.

S5 ne cherche pas à ajouter de nouvelles fonctions métier Modbus.

S5 ne constitue pas une qualification matérielle du bus RS-485.

## 3. Tranches S5 gelées

S5 couvre les tranches validées suivantes :

```text
S5-A   startup multi-bus atomique
S5-B   host console exécutable
S5-C   configuration opératoire et diagnostics de configuration
S5-D   diagnostics locaux minimaux
S5-E   recovery/restart B5 durable
S5-F   robustesse / stress logiciel pré-matériel
S5-G   audit final et clôture documentaire
```

## 4. Startup multi-bus atomique

Lors du startup, les bus série configurés sont ouverts avant que la readiness globale soit déclarée.

Invariant S5 :

```text
startup global réussi
OU
aucune connexion série ouverte pendant ce startup n'est conservée
```

Si l'ouverture d'un bus série ultérieur échoue, les connexions ouvertes précédemment pendant ce même startup sont libérées avant propagation de l'erreur.

La readiness n'est marquée qu'après recovery durable et ouverture complète des bus configurés.

Un journal B5 incohérent fait échouer le startup avant readiness.

## 5. Host console exécutable

Le point d'entrée opératoire courant est :

```text
TR2.Supervision.Service --config <path>
```

Aucun chemin de configuration par défaut n'est implicite.

Le host console :

- charge la configuration ;
- compose le runtime physique ;
- exécute le host long-running ;
- intercepte `Ctrl+C` comme cancellation demandée ;
- libère les ressources au shutdown.

Codes de sortie gelés comme **SUPERVISION_POLICY** :

```text
0  arrêt normal ou cancellation demandée
1  défaillance runtime
2  erreur d'usage ou de configuration
```

S5 ne transforme pas ce host en Windows Service.

## 6. Version protocole supportée côté supervision

La composition console sélectionne explicitement :

```text
supportedProtocolVersion = 1
```

Cette valeur est une **SUPERVISION_POLICY de compatibilité V1** côté PC.

Elle ne doit pas être interprétée comme une invention d'une valeur normative du registre `protocol_version` lorsque la spécification V1 ne fournit pas elle-même cette valeur numérique dans son texte normatif.

La règle reste : une identification B0 incompatible doit conduire à un comportement dégradé/refusé propre, sans importer silencieusement une règle V1.1.

## 7. Configuration opératoire

La configuration est fournie par fichier JSON explicite.

Un exemple est fourni par :

`Modbus RTU/Supervision TR2/tr2-supervision.example.json`

La documentation opératoire est :

`Modbus RTU/Supervision TR2/CONFIGURATION_OPERATOIRE.md`

Identités distinctes :

```text
bus.id           identité logique du bus supervision
serial.portName  port série physique
endpoints[]      adresses Modbus des équipements sur le bus
```

Le port COM ne devient jamais une identité d'équipement.

Pour un bus série, la configuration expose :

- `portName` ;
- `baudRate` ;
- `dataBits` ;
- `parity` ;
- `stopBits` ;
- `responseTimeoutMilliseconds`.

Valeurs de `parity` reconnues :

```text
None
Odd
Even
Mark
Space
```

Valeurs de `stopBits` reconnues :

```text
One
Two
OnePointFive
```

Toutes les valeurs d'exemple série (`COM7`, `115200`, `8N1`, `750 ms`, adresse `1`) restent illustratives et non normatives.

Le chemin SQLite relatif est résolu relativement au répertoire du fichier de configuration.

## 8. Diagnostics locaux

S5 ajoute des diagnostics console sans Web API ni Web UI.

Une fois le runtime réellement `Running`, le diagnostic local peut exposer :

- état du host ;
- readiness ;
- bus actuellement connectés ;
- endpoints configurés ;
- état de session de chaque endpoint ;
- `device_id` lorsqu'il est connu ;
- dernières défaillances de communication persistées.

La lecture des défaillances est bornée ; la console affiche au maximum les 10 plus récentes.

Cette borne est **SUPERVISION_POLICY**.

Aucune nouvelle taxonomie d'erreur n'est introduite en S5.

La taxonomie S4 reste :

```text
Unclassified
Timeout
Io
ModbusExceptionResponse
```

## 9. Recovery B5 au redémarrage

S5 vérifie au niveau du runtime les sémantiques durables déjà gelées par S1/S2/S3.

Ordre conceptuel du startup :

```text
ouvrir / migrer SQLite
        ↓
énumérer les device_id du journal B5
        ↓
reconstruire les coordinators
        ↓
rejouer logiquement le journal en mémoire uniquement
        ↓
ouvrir les bus série configurés
        ↓
mark ready
```

Règles gelées :

- historique terminal : aucune transaction active reconstruite ;
- tout état durable B5 non terminal : transaction active reconstruite en `Ambiguous` ;
- une transaction reconstruite active bloque une nouvelle commande ;
- aucun submit Modbus n'est rejoué au startup ;
- aucune nouvelle ligne de journal B5 n'est écrite simplement pour effectuer le recovery ;
- une transaction déjà ambiguë reste ambiguë ;
- l'allocation suivante de `transaction_id` continue sans réutilisation après historique terminal ;
- plusieurs devices sont reconstruits indépendamment ;
- un historique transactionnel incohérent provoque l'échec du startup avant readiness ;
- aucun `transaction_epoch` V1.1 n'est introduit.

En particulier, S5 ne remplace pas cette règle par « Prepared reste Prepared après redémarrage » : cette interprétation serait contraire aux arbitrages de recovery déjà gelés.

## 10. Robustesse scheduler et runtime

S5 exerce le `BusWorkScheduler` sous charge soutenue de travaux polling et prioritaires sur plusieurs bus.

Invariant conservé :

```text
un seul travail actif par bus logique
```

Le scheduler reste l'unique autorité de sérialisation du travail par bus.

Le runtime est également exercé avec plusieurs bus logiques et plusieurs endpoints, polling prolongé et cancellation propre.

Ces tests sont des tests de robustesse logicielle déterministes ; ils ne constituent pas un benchmark de performance ni une preuve de parallélisme matériel.

## 11. Robustesse SQLite

S5 ne change pas le schéma SQLite :

```text
PRAGMA user_version = 6
```

La configuration SQLite héritée reste notamment :

```text
journal_mode = WAL
synchronous = FULL
foreign_keys = ON
busy_timeout configuré
pooling activé
```

La robustesse du journal de communication est exercée par :

- centaines d'append successifs ;
- réouverture de la base ;
- contrôle du nombre exact de lignes ;
- contrôle append-only ;
- mélange d'opérations et catégories existantes ;
- plusieurs writers concurrents ;
- reader concurrent ;
- contrôle d'unicité des entrées attendues ;
- contrôle `PRAGMA integrity_check = ok`.

S5 ne déduit de ces tests aucune garantie sur une coupure électrique réelle du PC pendant écriture disque.

## 12. Shutdown et lifecycle

Le host reste one-shot.

Sur cancellation normale :

- readiness est retirée ;
- le host atteint `Stopped` ;
- les connexions série possédées sont disposées ;
- une nouvelle exécution du même objet host est refusée.

Une cancellation déjà demandée avant startup ne conduit pas à une readiness transitoire.

S5 n'ajoute aucun mécanisme de redémarrage automatique du processus lui-même.

## 13. Invariants S4 transport conservés

S5 préserve intégralement les invariants transport S4 :

- `busId` distinct du port COM et de l'identité équipement ;
- une connexion physique active maximum par bus ;
- `BusWorkScheduler` autorité de sérialisation ;
- aucun retry NModbus caché ;
- B5 sans replay automatique ;
- timeout et I/O conservant leurs sémantiques S4 ;
- I/O fermant la connexion et invalidant les sessions ;
- reconnexion nécessitant une nouvelle identification B0 ;
- réponse d'exception Modbus valide non assimilée automatiquement à une rupture physique ;
- ambiguïté B5 persistante à travers reconnexion et redémarrage tant qu'elle n'est pas résolue explicitement ;
- B6 non transformé en fausse transaction B5.

## 14. Classification des décisions S5

S5 ne modifie aucun comportement normatif Modbus RTU V1.

Relèvent de **SUPERVISION_POLICY** :

- syntaxe CLI `--config <path>` ;
- absence de chemin config implicite ;
- codes de sortie `0/1/2` ;
- sélection explicite de compatibilité protocole V1 côté supervision ;
- messages de diagnostic opérateur ;
- résumé de configuration au startup ;
- borne de 10 défaillances récentes affichées ;
- format du fichier de configuration opératoire ;
- politique de démarrage global atomique ;
- scénarios de stress logiciel pré-matériel.

Restent hérités des gels antérieurs :

- sémantique B5 durable/recovery ;
- mapping protocolaire B0 à B7 ;
- politique transport sans replay ;
- schéma SQLite 6 ;
- taxonomie de communication S4.

S5 ne crée aucune nouvelle **FW_POLICY**.

Toute lacune V1 reste `NOT_DEFINED V1` tant qu'elle n'est pas arbitrée par l'autorité appropriée.

Aucune règle V1.1 n'est importée silencieusement.

## 15. Validation obtenue

La baseline logicielle S5 validée couvre notamment :

- rollback d'un startup série multi-bus partiellement réussi ;
- host console exécutable ;
- erreurs usage/configuration/runtime ;
- configuration opératoire ;
- diagnostic runtime local ;
- lecture bornée des défaillances persistées ;
- recovery B5 après redémarrage ;
- états non terminaux reconstruits en `Ambiguous` ;
- blocage d'une nouvelle commande par transaction reconstruite ;
- absence de replay/journal mutation au startup ;
- allocation B5 après historique terminal ;
- recovery indépendant de plusieurs devices ;
- rejet d'un journal B5 incohérent avant readiness ;
- stress scheduler multi-bus ;
- runtime multi-bus/multi-endpoints ;
- cancellation et shutdown propres ;
- journalisation de communication soutenue ;
- contention SQLite writers/readers ;
- intégrité SQLite après stress.

## 16. Ce que S5 ne valide pas

S5 est explicitement **pré-matériel**.

Il ne valide pas :

- adaptateur USB/RS-485 réel ;
- chipset ou driver précis ;
- port COM réel ;
- niveaux électriques RS-485 ;
- polarité A/B ;
- référence commune ;
- terminaison ;
- polarisation/bias ;
- comportement physique du transceiver ;
- timing réel du bus ;
- paramètres série réels TR2 ;
- comportement réel d'un hot-unplug USB ;
- STM32/NUCLEO réel ;
- firmware flashé sur cible ;
- compatibilité électrique et temporelle PC ↔ adaptateur ↔ STM32 ;
- coupure secteur réelle du PC pendant écriture SQLite ;
- endurance disque réelle de longue durée.

Aucune conclusion matérielle ne doit être déduite des doubles logiciels S4/S5.

## 17. Hors périmètre après S5

Restent notamment ouverts :

- qualification et choix final de l'adaptateur USB/RS-485 ;
- validation physique avec cible STM32/TR2 ;
- paramètres série réels et procédure de provisioning ;
- version minimale exacte de Windows ;
- mode de publication .NET final ;
- packaging/installer ;
- Windows Service ;
- compte et droits de service ;
- Web API / Web UI ;
- authentification et rôles ;
- write path B4 supplémentaire si besoin futur démontré ;
- import/parser des campagnes SD ;
- stockage brut campagne ;
- FFT et analyses vibratoires ;
- rétention ;
- sauvegarde/restauration ;
- export ;
- chiffrement ;
- synchronisation analytique/Grafana ;
- transport réseau/UDP éventuel uniquement si un besoin concret apparaît.

## 18. Prérequis pour commencer le banc RS-485 réel

La prochaine campagne matérielle ne doit débuter que lorsque les éléments nécessaires sont disponibles et identifiés.

Prérequis minimaux :

1. cible STM32/NUCLEO retenue et firmware compatible disponible ;
2. adaptateur USB/RS-485 réel identifié ;
3. driver OS correspondant installé/qualifié ;
4. câblage RS-485 documenté, avec A/B et référence commune vérifiés ;
5. terminaison/polarisation décidées selon le montage réel ;
6. paramètres série réels établis depuis l'autorité firmware/protocole applicable ;
7. adresse Modbus réelle du TR2 connue ;
8. fichier de configuration PC créé avec ces valeurs réelles ;
9. méthode d'observation des trames/timings prévue ;
10. plan de test séparant clairement résultats logiciels déjà gelés et observations physiques nouvelles.

Le premier banc ne doit pas utiliser les valeurs illustratives de `tr2-supervision.example.json` comme si elles étaient des paramètres matériels validés.

## 19. Critère de sortie S5

S5 est considéré réussi lorsque, côté logiciel PC :

```text
TR2.Supervision.Service --config <path>
```

permet de garantir que :

- la configuration est chargée ou rejetée explicitement ;
- le runtime se compose sans I/O cachée ;
- la base est ouverte/migrée/récupérée avant readiness ;
- les bus série sont gérés atomiquement au startup ;
- les erreurs principales sont diagnostiquées localement ;
- les transactions B5 survivent au redémarrage selon les sémantiques gelées ;
- aucun replay B5 automatique n'est introduit ;
- le shutdown libère les ressources ;
- les principaux scénarios de charge logicielle pré-matériel ne révèlent pas de deadlock, perte évidente de journal ou corruption SQLite dans les tests validés.

À ce stade, le principal inconnu restant dans la chaîne Modbus RTU est **le comportement matériel réel RS-485/STM32**, et non la composition de base du runtime PC.

## 20. Règle de poursuite

Toute suite doit préserver les gels S0 à S5.

En particulier :

- ne jamais utiliser un port COM comme identité d'équipement ;
- ne jamais introduire un retry/replay B5 naïf ;
- ne jamais rendre ready un runtime dont le recovery durable a échoué ;
- ne jamais considérer une reconnexion physique comme une résolution d'ambiguïté B5 ;
- ne jamais déduire des paramètres série réels depuis les valeurs d'exemple ;
- ne jamais revendiquer une validation RS-485 matérielle à partir des tests logiciels S4/S5 ;
- maintenir `PREREQUIS_PC_SUPERVISION.md` lors des prochaines tranches ;
- classifier toute nouvelle décision comme `normatif V1`, `SUPERVISION_POLICY`, `FW_POLICY`, `NOT_DEFINED V1` ou V1.1 selon son autorité.

S5 est considéré gelé après intégration du présent document sur `main`, sur la base de la dernière baseline logicielle validée indiquée en section 1.
