# FT-LIM-10 — Cas génériques

## LIM10-G01 — Domaine statut global
PASS si `B3_STATUS_GLOBAL` ∈ {0..5}.

## LIM10-G02 — Réserve validity flags
PASS si `(B3_VALIDITY_FLAGS & 0xE000)==0`.

## LIM10-G03 — Réserve alarm flags
PASS si `(B3_ALARM_FLAGS & 0xFE00)==0`.

## LIM10-G04 — Domaine sévérité
PASS si `B3_SEVERITY_GLOBAL` ∈ {0..5}.

## LIM10-G05 — Domaine axe dominant
PASS si `B3_DOMINANT_AXIS` ∈ {0..4}.

## LIM10-G06 — Domaines dépassement
PASS si chacun des quatre `B3_EXCEED_*` ∈ {0,1}.

## LIM10-G07 — Domaine alarme mémorisée
PASS si `B3_ALARM_LATCHED` ∈ {0,1}.

## LIM10-G08 — Monotonie compteurs
Sans reset/statistics reset identifié, deux observations doivent satisfaire `EXCEED_COUNT2 >= EXCEED_COUNT1` et `ALARM_COUNT2 >= ALARM_COUNT1`.

## LIM10-G09 — Saturation compteurs
Si un compteur atteint `0xFFFFFFFF`, son maintien à cette valeur est conforme. Ne pas exiger de wrap-around.

## LIM10-G10 — Registres réservés
PASS si 3040..3047 valent tous zéro.

## LIM10-G11 — Seuils de configuration active
Avec une configuration active connue et une fenêtre calculée, vérifier que le résultat observé est rattaché à cette configuration active, pas à une configuration seulement préparée.

## LIM10-G12 — Absence de rétroactivité
Modifier une configuration préparée sans l’activer. PASS si les résultats déjà calculés ne sont pas recalculés/réinterprétés rétroactivement à cause de cette préparation.

## LIM10-G13 — Prise d’effet après activation
Après activation valide d’une nouvelle configuration, le changement de seuil ne s’applique qu’à partir de la prochaine fenêtre de calcul validée. Test conditionnel nécessitant une situation observable discriminante.

## LIM10-G14 — Dernière valeur conservée
Provoquer/observer une indisponibilité temporaire si possible. Une valeur numérique peut rester présente ; le test ne la déclare pas valide sur cette seule base et contrôle la présence d’une qualification de confiance appropriée sans imposer un masque exact non spécifié.

## LIM10-G15 — Convention d’unité V1
Vérifier la traçabilité/mapping : RMS, crêtes et seuils associés sont exprimés en accélération mg ; aucun champ V1 du Bloc 3 n’expose une vitesse mm/s.

## LIM10-G16 — Relations non définies
Consigner les relations status/flags/severity/exceed/alarm, fraîcheur exacte et relations algébriques non normées en TRACE_ONLY/NOT_DEFINED plutôt que d’inventer un oracle.
