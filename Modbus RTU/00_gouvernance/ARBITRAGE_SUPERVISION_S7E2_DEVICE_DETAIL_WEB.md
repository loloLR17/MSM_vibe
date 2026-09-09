# S7-E2 — Écran réel détail TR2 / vibrations / diagnostic

## Statut

Arbitrage d’implémentation supervision PC.

Cette tranche est **read-only** et s’appuie exclusivement sur les projections validées en S7-E1.

## Objet

Fournir un écran Web de détail pour un TR2 identifié par son `device_id`, accessible depuis la vue flotte S7-D.

## Source d’autorité

L’écran consomme uniquement :

`GET /api/v1/devices/{deviceId}`

Le navigateur ne lit jamais Modbus directement et ne maintient aucune autorité métier parallèle.

## Navigation

Depuis la vue générale, un endpoint possédant un `device_id` connu devient navigable vers :

`/device.html?deviceId=<id>`

Un endpoint non identifié n’invente pas de `device_id` et ne propose pas de lien de détail.

## Contenu affiché

L’écran détail présente quatre vues locales :

- Synthèse ;
- Vibrations ;
- État système ;
- Diagnostic.

Les données proviennent exclusivement de B1, B2, B3 et B7 déjà projetés.

### Synthèse

Affichage de :

- endpoint bus / adresse ;
- `device_id` ;
- état de session ;
- fraîcheur PC B1 / B3 / B7 ;
- RMS global B3 ;
- crête globale B3 ;
- statut système B1 ;
- statut santé B7.

### Vibrations

Affichage des valeurs B3 fournies par le TR2 :

- RMS global ;
- crête globale ;
- RMS X/Y/Z ;
- crête X/Y/Z ;
- statut global ;
- sévérité globale ;
- axe dominant ;
- flags validité ;
- flags alarme ;
- âge de valeur ;
- compteurs de dépassement et d’alarme.

Aucune grandeur n’est recalculée par le navigateur.

### État système

Affichage read-only de B1 et B2 : état, flags, acquisition, campagne active, uptime, température interne, CPU, mémoire, stockage et informations temporelles projetées.

### Diagnostic

Affichage read-only du Bloc 7 : version structure, état santé, flags défaut, dernier défaut, autotest, uptime, cause reset, température interne et tension alimentation.

## Règles de représentation

Les valeurs numériques protocolaires restent visibles comme valeurs de référence. Les libellés ajoutés côté Web ne concernent que des enums explicitement définis par la V1 ; une valeur hors domaine connu est affichée comme `Réservé / valeur N` et n’est pas interprétée.

La fraîcheur PC reste distincte de la validité interne B3.

En cas de perte HTTP, les dernières données déjà affichées sont conservées et une erreur Web explicite est présentée.

En cas de snapshot indisponible, la dernière valeur connue peut rester visible avec son état `Unavailable` ; elle n’est jamais remise artificiellement à zéro.

## Exclusions

S7-E2 n’introduit pas :

- B4 configuration dans l’IHM ;
- B5 write path ;
- B6 campagnes ;
- action SELFTEST ;
- résolution `Ambiguous` ;
- FFT ;
- vitesse mm/s ;
- fréquence dominante ;
- seuil ISO ;
- score santé calculé ;
- seuil métier inventé ;
- authentification / rôles.

## Tests attendus

Les tests vérifient au minimum :

- la page statique de détail est servie ;
- elle référence l’API device S7-E1 ;
- elle ne contient aucune commande B5 ;
- la vue générale ne fabrique un lien détail que lorsque `device_id` est présent.

## Validation

La tranche n’est considérée validée qu’après compilation et exécution locale complète des tests par l’opérateur.
