# Arbitrage supervision S7-F3 — Système et communications Web

## Statut

Décision d’implémentation supervision PC, tranche S7-F3.

## Objet

Exposer dans l’IHM Web l’état du runtime de supervision et les derniers échecs de communication déjà persistés, sans créer de nouvel état autoritatif ni de second journal.

## Décisions

- `RuntimeDiagnosticSnapshot.Capture(...)` reste la source runtime pour l’état hôte, la readiness, les bus connectés et les sessions/endpoints.
- `SqliteCommunicationJournalSink.ReadLatest(...)` reste la source persistante pour les échecs de communication.
- La lecture Web est bornée aux 20 derniers échecs de communication.
- `TR2.Supervision.Web` ne référence pas `TR2.Supervision.Service` : une interface read-only et des DTO IHM sont définis côté Web, et un adaptateur est implémenté côté Service.
- Aucun cache Web parallèle n’est ajouté.
- `GET /api/v1/system` expose un snapshot ponctuel accompagné de `observedAt` côté PC.
- La page `/system.html` consomme uniquement cette API.
- Les messages et noms d’exception persistés sont rendus comme texte DOM, jamais comme HTML.
- Les échecs de communication PC restent distincts des défauts et warnings TR2.
- L’absence d’échec dans la fenêtre lue signifie uniquement « aucun échec parmi les enregistrements retournés », pas une preuve d’absence historique d’incident.

## Données exposées

- état du host de supervision ;
- readiness ;
- identifiants des bus actuellement connectés ;
- endpoints configurés avec état de session et `device_id` lorsqu’il est connu ;
- derniers échecs de communication : identifiant, bus/adresse, `device_id`, opération, catégorie, timestamp, type d’exception et message.

## Hors périmètre

- commandes runtime depuis le Web ;
- reconnexion forcée ;
- acquittement ou suppression d’un événement de communication ;
- agrégations statistiques ou score de santé inventé ;
- confusion entre défaut communication PC et défaut TR2 ;
- B5, B6 write path et authentification.

## Suite

Après validation locale de S7-F3, la tranche S7-F peut poursuivre sur les éventuels compléments de navigation/diagnostic nécessaires avant S7-G, puis ouvrir le write path B5 uniquement à travers l’autorité opérationnelle existante.