# Arbitrage supervision S7-G3 — Actions B5 dans l’IHM Web

## Statut

Décision d’implémentation supervision PC, tranche S7-G3.

## Objet

Rendre les commandes B5 déjà autorisées par S7-G1/S7-G2 accessibles depuis la page TR2, sans créer de nouvelle autorité de commande et sans assimiler l’acceptation HTTP à un succès matériel.

## Invariants

- Le navigateur n’accède jamais directement au Modbus.
- Toutes les commandes passent par `POST /api/v1/devices/{deviceId}/commands` puis par l’autorité existante `SupervisionOperationalFacade.QueueCommandAsync(...)`.
- `requestIdentity` est généré côté navigateur pour chaque intention opérateur et n’est jamais réutilisé automatiquement.
- Aucun retry automatique n’est effectué en cas d’erreur réseau ou de résultat HTTP inconnu.
- Un HTTP 202 signifie uniquement que la transaction a été préparée et mise en file. Il ne signifie pas que le TR2 l’a exécutée avec succès.
- `RESET_STATISTICS` reste absent.
- Aucun paramètre B5 brut n’est exposé.

## Actions exposées

La vue `Commandes` propose :

- appliquer la configuration préparée ;
- synchroniser l’heure préparée ;
- démarrer l’acquisition ;
- arrêter l’acquisition ;
- lancer l’autotest standard ;
- rafraîchir les indicateurs ;
- entrer en maintenance ;
- sortir de maintenance ;
- acquitter un défaut unitaire par code ;
- acquitter globalement les défauts acquittables ;
- reset logiciel contrôlé.

## Confirmation opérateur

- Toute commande est déclenchée uniquement par une action explicite de l’opérateur.
- Les commandes courantes utilisent une confirmation simple avant émission.
- L’acquittement global affiche une confirmation spécifique indiquant qu’il vise tous les défauts acquittables.
- `SOFTWARE_RESET` utilise une confirmation renforcée et envoie `confirmProtectedCommand=true`; la clé normative `0xA55A` reste générée exclusivement dans le Service.

## Retour opérateur

Après HTTP 202, l’IHM affiche notamment le `transactionId` et rappelle que la commande est seulement mise en file.

En cas de rejet HTTP, le code et le message retournés par l’API sont affichés comme texte.

En cas d’erreur réseau, l’IHM indique que le résultat est inconnu et interdit toute interprétation automatique en échec ou toute retransmission automatique. Cette règle prépare S7-H, qui traitera explicitement les états transactionnels et `Ambiguous`.

## Hors périmètre

- lecture de l’historique transactionnel B5 dans l’IHM ;
- résolution de `Ambiguous` ;
- replay/retry automatique ;
- sélection B6 ;
- authentification, rôles et ACL ;
- édition de configuration B4 ;
- `RESET_STATISTICS`.

## Validation attendue

- la page TR2 expose une vue `Commandes` ;
- le script utilise exclusivement l’API sémantique S7-G1/G2 ;
- `AcknowledgeFault` respecte les formes unitaire/globale ;
- `SoftwareReset` nécessite confirmation et `confirmProtectedCommand=true` ;
- aucun `ResetStatistics`, registre B5 brut ou retry automatique n’est présent ;
- les autres vues et lectures restent inchangées.
