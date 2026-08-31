# FT-BLK-06 — Stabilité, identité et délégations

## 1. Objet

FT-BLK-06 ferme la couverture fonctionnelle intra-bloc V1 sur deux zones volontairement résiduelles :

- Bloc 0 : stabilité fonctionnelle des informations d'identification pendant le fonctionnement normal ;
- Bloc 5 : inventaire et délégation explicite des exigences du moteur de commandes vers FT-CMD.

Cette sous-famille n'est pas un substitut à FT-CMD.

## 2. Doctrine

Pour le Bloc 0, FT-BLK-06 vérifie uniquement les invariants fonctionnels qui ne sont ni structurels, ni des règles d'accès, ni des règles de persistance après redémarrage.

Pour le Bloc 5, les exigences transactionnelles sont tracées mais ne sont pas réexécutées dans FT-BLK : le plan de test réserve ces comportements à FT-CMD.

## 3. Périmètre B0

Sont couverts ou classifiés :

- stabilité des informations d'identification pendant le fonctionnement normal ;
- absence de dépendance des champs B0 à un état dynamique du capteur ;
- unicité de `device_id` : conditionnelle à un parc d'au moins deux équipements ;
- persistance de `device_id` : déléguée à FT-PER.

Les règles ASCII, MSW/LSW, réservés et cohérence de lecture relèvent de FT-STR. Le caractère RO et le rejet des écritures relèvent de FT-ACC.

## 4. Périmètre B5

Le Bloc 5 est explicitement inventorié comme domaine fonctionnel délégué à FT-CMD :

- déclenchement sur front montant `submit` ;
- non-réexécution lorsque `submit` reste à 1 ;
- `transaction_id` et corrélation requête/réponse ;
- idempotence d'une transaction déjà traitée ;
- une seule commande active à la fois ;
- états, résultats et historique de commande ;
- commandes protégées et clé de confirmation ;
- annulation / nettoyage selon les règles V1 ;
- effets métier des commandes sur les autres blocs.

FT-BLK-06 ne crée aucun test concurrent à FT-CMD pour ces exigences.

## 5. Artefacts

- `source/FT-BLK-06_source.md`
- `detaille/FT-BLK-06_detaille.md`
- `detaille/FT-BLK-06_matrice_couverture.csv`

## 6. Statut

Sous-famille reconstruite sur le cadrage validé. Gel interdit avant audit croisé final de FT-BLK-01 à FT-BLK-06 et validation explicite.
