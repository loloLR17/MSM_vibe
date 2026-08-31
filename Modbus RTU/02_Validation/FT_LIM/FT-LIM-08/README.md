# FT-LIM-08 — Identification et état système

## Objet
Valider les domaines et invariants fonctionnels explicitement normatifs des Blocs 0 et 1.

## Périmètre
- bits réservés de `device_capabilities` ;
- stabilité des informations d’identification du Bloc 0 en fonctionnement normal ;
- persistance du `device_id` et unicité conditionnelle entre équipements ;
- `last_reset_cause` : domaine 0..6 ;
- `storage_status` : domaine 0..3 ;
- `acquisition_state` : domaine 0..3 ;
- monotonie de `uptime_s` ;
- persistance observable des défauts/avertissements tant que leur condition reste présente.

## Hors périmètre
- permissions RO : FT-ACC ;
- longueurs ASCII, types, MSW/LSW, cohérence multi-registres : FT-STR ;
- domaine détaillé de `system_status` : non défini en V1 après correction normative ;
- sémantique détaillée des flags et codes erreur/avertissement : non définie ;
- plages fonctionnelles de température et champs pourcentage : non définies explicitement.

Aucun état RO n’est fabriqué par écriture directe.
